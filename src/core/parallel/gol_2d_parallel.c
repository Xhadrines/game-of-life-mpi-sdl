#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "../include/utils/utils.h"
#include "../include/ui/sdl_viewer.h"
#include "../include/core/parallel/gol_2d_parallel.h"

static int owner_rows(int rows, int size, int rank) {
    int base = rows / size;
    int rem = rows % size;

    return base + (rank < rem ? 1 : 0);
}

static int owner_offset(int rows, int size, int rank) {
    int offset = 0;

    for (int r = 0; r < rank; r++) {
        offset += owner_rows(rows, size, r);
    }

    return offset;
}

static int alive_at(const unsigned char *grid, int local_rows, int cols, int r, int c) {
    if (c < 0 || c >= cols) return 0;
    if (r < 0 || r >= local_rows + 2) return 0;

    return grid[r * cols + c] ? 1 : 0;
}

static void step_2d(unsigned char *current, unsigned char *next, int local_rows, int cols) {
    for (int r = 1; r <= local_rows; r++) {
        for (int c = 0; c < cols; c++) {
            int neighbors = 0;

            neighbors += alive_at(current, local_rows, cols, r - 1, c - 1);
            neighbors += alive_at(current, local_rows, cols, r - 1, c);
            neighbors += alive_at(current, local_rows, cols, r - 1, c + 1);

            neighbors += alive_at(current, local_rows, cols, r, c - 1);
            neighbors += alive_at(current, local_rows, cols, r, c + 1);

            neighbors += alive_at(current, local_rows, cols, r + 1, c - 1);
            neighbors += alive_at(current, local_rows, cols, r + 1, c);
            neighbors += alive_at(current, local_rows, cols, r + 1, c + 1);

            unsigned char cell = current[r * cols + c];

            if (cell && (neighbors == 2 || neighbors == 3)) {
                next[r * cols + c] = 1;
            } else if (!cell && neighbors == 3) {
                next[r * cols + c] = 1;
            } else {
                next[r * cols + c] = 0;
            }
        }
    }
}

static void run_gol_2d_internal(
    int rows,
    int cols,
    int steps,
    const char *out_path,
    int use_sdl,
    int scale,
    int delay_ms
) {
    int rank, size;

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    int local_rows = owner_rows(rows, size, rank);

    unsigned char *global = NULL;
    int *sendcounts = NULL;
    int *displs = NULL;

    if (rank == 0) {
        global = alloc_grid(rows, cols);

        sendcounts = malloc((size_t)size * sizeof(int));
        displs = malloc((size_t)size * sizeof(int));

        if (!global || !sendcounts || !displs) {
            fprintf(stderr, "Eroare alocare memorie pe rank 0.\n");
            MPI_Abort(MPI_COMM_WORLD, 1);
        }

        random_fill(global, rows, cols, 0.30, (unsigned int)time(NULL));

        for (int p = 0; p < size; p++) {
            int lr = owner_rows(rows, size, p);
            int off = owner_offset(rows, size, p);

            sendcounts[p] = lr * cols;
            displs[p] = off * cols;
        }
    }

    unsigned char *current = alloc_grid(local_rows + 2, cols);
    unsigned char *next = alloc_grid(local_rows + 2, cols);

    if (!current || !next) {
        fprintf(stderr, "Eroare alocare memorie pe rank %d.\n", rank);
        MPI_Abort(MPI_COMM_WORLD, 1);
    }

    MPI_Scatterv(
        global,
        sendcounts,
        displs,
        MPI_UNSIGNED_CHAR,
        &current[cols],
        local_rows * cols,
        MPI_UNSIGNED_CHAR,
        0,
        MPI_COMM_WORLD
    );

    int up = rank == 0 ? MPI_PROC_NULL : rank - 1;
    int down = rank == size - 1 ? MPI_PROC_NULL : rank + 1;

    int running = 1;

    if (rank == 0 && use_sdl) {
        running = sdl_init_viewer(rows, cols, scale);
    }

    MPI_Bcast(&running, 1, MPI_INT, 0, MPI_COMM_WORLD);

    double start_time = MPI_Wtime();

    for (int step = 0; step < steps && running; step++) {
        MPI_Sendrecv(
            &current[cols],
            cols,
            MPI_UNSIGNED_CHAR,
            up,
            100,
            &current[(local_rows + 1) * cols],
            cols,
            MPI_UNSIGNED_CHAR,
            down,
            100,
            MPI_COMM_WORLD,
            MPI_STATUS_IGNORE
        );

        MPI_Sendrecv(
            &current[local_rows * cols],
            cols,
            MPI_UNSIGNED_CHAR,
            down,
            200,
            &current[0],
            cols,
            MPI_UNSIGNED_CHAR,
            up,
            200,
            MPI_COMM_WORLD,
            MPI_STATUS_IGNORE
        );

        memset(next, 0, (size_t)(local_rows + 2) * (size_t)cols);

        step_2d(current, next, local_rows, cols);

        unsigned char *tmp = current;
        current = next;
        next = tmp;

        if (use_sdl) {
            MPI_Gatherv(
                &current[cols],
                local_rows * cols,
                MPI_UNSIGNED_CHAR,
                global,
                sendcounts,
                displs,
                MPI_UNSIGNED_CHAR,
                0,
                MPI_COMM_WORLD
            );

            if (rank == 0) {
                running = sdl_render_grid(
                    global,
                    rows,
                    cols,
                    delay_ms,
                    "2D",
                    step + 1,
                    steps
                );
            }

            MPI_Bcast(&running, 1, MPI_INT, 0, MPI_COMM_WORLD);
        }
    }

    MPI_Gatherv(
        &current[cols],
        local_rows * cols,
        MPI_UNSIGNED_CHAR,
        global,
        sendcounts,
        displs,
        MPI_UNSIGNED_CHAR,
        0,
        MPI_COMM_WORLD
    );

    double end_time = MPI_Wtime();
    double elapsed = end_time - start_time;

    if (rank == 0) {
        if (write_pgm(out_path, global, rows, cols)) {
            printf("Timp parallel 2D: %.6f secunde\n", elapsed);
            printf("Imagine salvata: %s\n", out_path);
        }

        if (use_sdl) {
            sdl_wait_for_enter("SIMULARE PARALELA FINALIZATA", elapsed);
            sdl_close_viewer();
        }
    }

    free(current);
    free(next);

    if (rank == 0) {
        free(global);
        free(sendcounts);
        free(displs);
    }
}

void run_gol_2d_parallel(int rows, int cols, int steps, const char *out_path) {
    run_gol_2d_internal(rows, cols, steps, out_path, 0, 1, 0);
}

void run_gol_2d_parallel_visual(int rows, int cols, int steps, const char *out_path, int scale, int delay_ms) {
    if (scale <= 0) scale = 4;
    if (delay_ms < 0) delay_ms = 20;

    run_gol_2d_internal(rows, cols, steps, out_path, 1, scale, delay_ms);
}
