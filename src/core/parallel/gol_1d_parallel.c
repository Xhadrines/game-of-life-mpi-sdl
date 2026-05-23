#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "../include/utils/utils.h"
#include "../include/ui/sdl_viewer.h"
#include "../include/core/parallel/gol_1d_parallel.h"

static int owner_cells(int n, int size, int rank) {
    int base = n / size;
    int rem = n % size;

    return base + (rank < rem ? 1 : 0);
}

static int owner_offset(int n, int size, int rank) {
    int offset = 0;

    for (int r = 0; r < rank; r++) {
        offset += owner_cells(n, size, r);
    }

    return offset;
}

static unsigned char rule30(unsigned char left, unsigned char center, unsigned char right) {
    int pattern = (left << 2) | (center << 1) | right;

    return (30 >> pattern) & 1;
}

static void run_gol_1d_internal(
    int global_n,
    int steps,
    const char *out_path,
    int use_sdl,
    int scale,
    int delay_ms
) {
    int rank, size;

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    int local_n = owner_cells(global_n, size, rank);

    unsigned char *global_initial = NULL;
    unsigned char *history = NULL;
    int *sendcounts = NULL;
    int *displs = NULL;

    if (rank == 0) {
        global_initial = alloc_grid(1, global_n);
        history = alloc_grid(steps + 1, global_n);

        sendcounts = malloc((size_t)size * sizeof(int));
        displs = malloc((size_t)size * sizeof(int));

        if (!global_initial || !history || !sendcounts || !displs) {
            fprintf(stderr, "Eroare alocare memorie.\n");
            MPI_Abort(MPI_COMM_WORLD, 1);
        }

        random_fill(global_initial, 1, global_n, 0.50, (unsigned int)time(NULL));

        for (int p = 0; p < size; p++) {
            sendcounts[p] = owner_cells(global_n, size, p);
            displs[p] = owner_offset(global_n, size, p);
        }
    }

    unsigned char *current = calloc((size_t)local_n + 2, sizeof(unsigned char));
    unsigned char *next = calloc((size_t)local_n + 2, sizeof(unsigned char));

    if (!current || !next) {
        fprintf(stderr, "Eroare alocare memorie pe rank %d.\n", rank);
        MPI_Abort(MPI_COMM_WORLD, 1);
    }

    MPI_Scatterv(
        global_initial,
        sendcounts,
        displs,
        MPI_UNSIGNED_CHAR,
        &current[1],
        local_n,
        MPI_UNSIGNED_CHAR,
        0,
        MPI_COMM_WORLD
    );

    int left_rank = rank == 0 ? MPI_PROC_NULL : rank - 1;
    int right_rank = rank == size - 1 ? MPI_PROC_NULL : rank + 1;

    int running = 1;

    if (rank == 0 && use_sdl) {
        running = sdl_init_viewer(steps + 1, global_n, scale);
    }

    MPI_Bcast(&running, 1, MPI_INT, 0, MPI_COMM_WORLD);

    double start_time = MPI_Wtime();

    for (int step = 0; step <= steps && running; step++) {
        MPI_Gatherv(
            &current[1],
            local_n,
            MPI_UNSIGNED_CHAR,
            history ? &history[step * global_n] : NULL,
            sendcounts,
            displs,
            MPI_UNSIGNED_CHAR,
            0,
            MPI_COMM_WORLD
        );

        if (rank == 0 && use_sdl) {
            running = sdl_render_grid(
                history,
                steps + 1,
                global_n,
                delay_ms,
                "1D",
                step,
                steps
            );
        }

        MPI_Bcast(&running, 1, MPI_INT, 0, MPI_COMM_WORLD);

        if (step == steps || !running) break;

        MPI_Sendrecv(
            &current[1],
            1,
            MPI_UNSIGNED_CHAR,
            left_rank,
            10,
            &current[local_n + 1],
            1,
            MPI_UNSIGNED_CHAR,
            right_rank,
            10,
            MPI_COMM_WORLD,
            MPI_STATUS_IGNORE
        );

        MPI_Sendrecv(
            &current[local_n],
            1,
            MPI_UNSIGNED_CHAR,
            right_rank,
            20,
            &current[0],
            1,
            MPI_UNSIGNED_CHAR,
            left_rank,
            20,
            MPI_COMM_WORLD,
            MPI_STATUS_IGNORE
        );

        for (int i = 1; i <= local_n; i++) {
            next[i] = rule30(current[i - 1], current[i], current[i + 1]);
        }

        unsigned char *tmp = current;
        current = next;
        next = tmp;

        memset(next, 0, (size_t)local_n + 2);
    }

    double end_time = MPI_Wtime();
    double elapsed = end_time - start_time;

    if (rank == 0) {
        if (write_output_files(out_path, history, steps + 1, global_n)) {
            printf("Timp parallel 1D: %.6f secunde\n", elapsed);
            printf("Imagine PGM salvata: output/pgm/%s.pgm\n", out_path);
            printf("Imagine PPM salvata: output/ppm/%s.ppm\n", out_path);

            if ((steps + 1) <= TXT_EXPORT_MAX_ROWS && global_n <= TXT_EXPORT_MAX_COLS) {
                printf("Fisier TXT salvat: output/txt/%s.txt\n", out_path);
            }
        }

        if (use_sdl) {
            sdl_wait_for_enter("SIMULARE PARALELA FINALIZATA", elapsed);
            sdl_close_viewer();
        }
    }

    free(current);
    free(next);

    if (rank == 0) {
        free(global_initial);
        free(history);
        free(sendcounts);
        free(displs);
    }
}

void run_gol_1d_parallel(int global_n, int steps, const char *out_path) {
    run_gol_1d_internal(global_n, steps, out_path, 0, 1, 0);
}

void run_gol_1d_parallel_visual(int global_n, int steps, const char *out_path, int scale, int delay_ms) {
    if (scale <= 0) scale = 1;
    if (delay_ms < 0) delay_ms = 10;

    run_gol_1d_internal(global_n, steps, out_path, 1, scale, delay_ms);
}
