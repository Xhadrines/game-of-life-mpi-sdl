#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "../include/utils/utils.h"
#include "../include/utils/patterns.h"
#include "../include/ui/sdl_viewer.h"
#include "../include/core/parallel/gol_2d_parallel.h"

/*
 * Functie: owner_rows (static)
 * Ce face: calculeaza numarul de randuri atribuite fiecarui proces (imparsire cu rest).
 */
static int owner_rows(int rows, int size, int rank) {
    int base = rows / size;
    int rem = rows % size;

    return base + (rank < rem ? 1 : 0);
}

/*
 * Functie: owner_offset (static)
 * Ce face: calcul offset pentru randul de start al unui proces in grid-ul global.
 */
static int owner_offset(int rows, int size, int rank) {
    int offset = 0;

    for (int r = 0; r < rank; r++) {
        offset += owner_rows(rows, size, r);
    }

    return offset;
}

/*
 * Functie: alive_at (static)
 * Ce face: verifica daca o celula din buffer-ul local (cu ghost rows) este vie.
 */
static int alive_at(const unsigned char *grid, int local_rows, int cols, int r, int c) {
    if (c < 0 || c >= cols) return 0;
    if (r < 0 || r >= local_rows + 2) return 0;

    return grid[r * cols + c] ? 1 : 0;
}

/*
 * Functie: step_2d_range (static)
 * Ce face: calculeaza next pentru un range de randuri locale (folosit pentru overlap calcul-comunicasie).
 */
static void step_2d_range(
    unsigned char *current,
    unsigned char *next,
    int local_rows,
    int cols,
    int start_row,
    int end_row
) {
    for (int r = start_row; r <= end_row; r++) {
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

/*
 * Functie: run_gol_2d_internal (static)
 * Ce face: logica comuna pentru rularea simularii MPI 2D; gestioneaza scatter/gatherv,
 *          halo exchange non-blocking, masurare timp comunicasie vs calcul si export.
 * Parametri: rows, cols, steps, out_path, use_sdl, scale, delay_ms, pattern_type
 */
static void run_gol_2d_internal(
    int rows,
    int cols,
    int steps,
    const char *out_path,
    int use_sdl,
    int scale,
    int delay_ms,
    int pattern_type
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

        apply_pattern(global, rows, cols, pattern_type, (unsigned int)time(NULL));

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

    MPI_Comm cart_comm;

    int dims[1] = {size};
    int periods[1] = {0};
    int reorder = 0;

    MPI_Cart_create(
        MPI_COMM_WORLD,
        1,
        dims,
        periods,
        reorder,
        &cart_comm
    );

    int up, down;

    MPI_Cart_shift(
        cart_comm,
        0,
        1,
        &up,
        &down
    );

    int running = 1;

    if (rank == 0 && use_sdl) {
        running = sdl_init_viewer(rows, cols, scale);
    }

    MPI_Bcast(&running, 1, MPI_INT, 0, MPI_COMM_WORLD);

    double start_time = MPI_Wtime();

    double communication_time = 0.0;
    double computation_time = 0.0;

    MPI_Request requests[4];
    
    for (int step = 0; step < steps && running; step++) {
        double comm_start = MPI_Wtime();

        MPI_Irecv(
            &current[0],
            cols,
            MPI_UNSIGNED_CHAR,
            up,
            200,
            cart_comm,
            &requests[0]
        );

        MPI_Irecv(
            &current[(local_rows + 1) * cols],
            cols,
            MPI_UNSIGNED_CHAR,
            down,
            100,
            cart_comm,
            &requests[1]
        );

        MPI_Isend(
            &current[cols],
            cols,
            MPI_UNSIGNED_CHAR,
            up,
            100,
            cart_comm,
            &requests[2]
        );

        MPI_Isend(
            &current[local_rows * cols],
            cols,
            MPI_UNSIGNED_CHAR,
            down,
            200,
            cart_comm,
            &requests[3]
        );

        communication_time += MPI_Wtime() - comm_start;

        double comp_start = MPI_Wtime();

        memset(next, 0, (size_t)(local_rows + 2) * (size_t)cols);

        if (local_rows > 2) {
            step_2d_range(
                current,
                next,
                local_rows,
                cols,
                2,
                local_rows - 1
            );
        }

        computation_time += MPI_Wtime() - comp_start;

        comm_start = MPI_Wtime();

        MPI_Waitall(
            4,
            requests,
            MPI_STATUSES_IGNORE
        );

        communication_time += MPI_Wtime() - comm_start;

        comp_start = MPI_Wtime();

        if (local_rows >= 1) {
            step_2d_range(
                current,
                next,
                local_rows,
                cols,
                1,
                1
            );
        }

        if (local_rows >= 2) {
            step_2d_range(
                current,
                next,
                local_rows,
                cols,
                local_rows,
                local_rows
            );
        }

        computation_time += MPI_Wtime() - comp_start;

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
        if (write_output_files(out_path, global, rows, cols)) {
            double communication_percent = 0.0;

            if (elapsed > 0.0) {
                communication_percent = (communication_time / elapsed) * 100.0;
            }

            printf("Timp total simulare MPI 2D: %.6f secunde\n", elapsed);
            printf("Timp petrecut in comunicatia MPI halo exchange: %.6f secunde\n", communication_time);
            printf("Timp petrecut in calculul generatiilor Game of Life: %.6f secunde\n", computation_time);
            printf("Procent din timpul total folosit pentru comunicatie MPI: %.2f%%\n", communication_percent);

            append_benchmark_csv(
                out_path,
                "parallel2d",
                size,
                rows,
                cols,
                steps,
                elapsed,
                communication_time,
                computation_time
            );

            printf("Imagine PGM salvata: output/pgm/%s.pgm\n", out_path);
            printf("Imagine PPM salvata: output/ppm/%s.ppm\n", out_path);

            if (rows <= TXT_EXPORT_MAX_ROWS && cols <= TXT_EXPORT_MAX_COLS) {
                printf("Fisier TXT salvat: output/txt/%s.txt\n", out_path);
            }
        }

        if (use_sdl) {
            sdl_wait_for_enter("SIMULARE PARALELA FINALIZATA", elapsed);
            sdl_close_viewer();
        }
    }

    MPI_Comm_free(&cart_comm);

    free(current);
    free(next);

    if (rank == 0) {
        free(global);
        free(sendcounts);
        free(displs);
    }
}

/*
 * Functie: run_gol_2d_parallel
 * Ce face: wrapper pentru rulare MPI 2D non-vizuala (apeleaza run_gol_2d_internal).
 */
void run_gol_2d_parallel(int rows, int cols, int steps, const char *out_path, int pattern_type) {
    run_gol_2d_internal(rows, cols, steps, out_path, 0, 1, 0, pattern_type);
}

/*
 * Functie: run_gol_2d_parallel_visual
 * Ce face: wrapper pentru rulare MPI 2D cu vizualizare.
 */
void run_gol_2d_parallel_visual(int rows, int cols, int steps, const char *out_path, int scale, int delay_ms, int pattern_type) {
    if (scale <= 0) scale = 4;
    if (delay_ms < 0) delay_ms = 20;

    run_gol_2d_internal(rows, cols, steps, out_path, 1, scale, delay_ms, pattern_type);
}
