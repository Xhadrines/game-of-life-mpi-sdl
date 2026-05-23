#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "../../../include/utils/utils.h"
#include "../../../include/utils/patterns.h"
#include "../../../include/core/parallel/gol_2d_parallel_blocks.h"

static int idx(int r, int c, int width) {
    return r * width + c;
}

static int alive_at(
    const unsigned char *grid,
    int local_rows,
    int local_cols,
    int r,
    int c
) {
    if (r < 0 || r >= local_rows + 2) return 0;
    if (c < 0 || c >= local_cols + 2) return 0;

    return grid[idx(r, c, local_cols + 2)] ? 1 : 0;
}

static void step_blocks_range(
    unsigned char *current,
    unsigned char *next,
    int local_rows,
    int local_cols,
    int start_row,
    int end_row
) {
    int width = local_cols + 2;

    for (int r = start_row; r <= end_row; r++) {
        for (int c = 1; c <= local_cols; c++) {
            int neighbors = 0;

            neighbors += alive_at(current, local_rows, local_cols, r - 1, c - 1);
            neighbors += alive_at(current, local_rows, local_cols, r - 1, c);
            neighbors += alive_at(current, local_rows, local_cols, r - 1, c + 1);

            neighbors += alive_at(current, local_rows, local_cols, r, c - 1);
            neighbors += alive_at(current, local_rows, local_cols, r, c + 1);

            neighbors += alive_at(current, local_rows, local_cols, r + 1, c - 1);
            neighbors += alive_at(current, local_rows, local_cols, r + 1, c);
            neighbors += alive_at(current, local_rows, local_cols, r + 1, c + 1);

            unsigned char cell = current[idx(r, c, width)];

            if (cell && (neighbors == 2 || neighbors == 3)) {
                next[idx(r, c, width)] = 1;
            } else if (!cell && neighbors == 3) {
                next[idx(r, c, width)] = 1;
            } else {
                next[idx(r, c, width)] = 0;
            }
        }
    }
}

static void copy_global_to_local_block(
    const unsigned char *global,
    unsigned char *local,
    // int rows, --- IGNORE ---
    int cols,
    int local_rows,
    int local_cols,
    int row_offset,
    int col_offset
) {
    int local_width = local_cols + 2;

    for (int r = 0; r < local_rows; r++) {
        for (int c = 0; c < local_cols; c++) {
            local[idx(r + 1, c + 1, local_width)] =
                global[(row_offset + r) * cols + (col_offset + c)];
        }
    }
}

static void copy_local_block_to_global(
    unsigned char *global,
    const unsigned char *local,
    int cols,
    int local_rows,
    int local_cols,
    int row_offset,
    int col_offset
) {
    int local_width = local_cols + 2;

    for (int r = 0; r < local_rows; r++) {
        for (int c = 0; c < local_cols; c++) {
            global[(row_offset + r) * cols + (col_offset + c)] =
                local[idx(r + 1, c + 1, local_width)];
        }
    }
}

void run_gol_2d_parallel_blocks(
    int rows,
    int cols,
    int steps,
    const char *out_path,
    int pattern_type
) {
    int rank, size;

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    int dims[2] = {0, 0};
    int periods[2] = {0, 0};
    int coords[2];

    MPI_Dims_create(size, 2, dims);

    if (rows % dims[0] != 0 || cols % dims[1] != 0) {
        if (rank == 0) {
            fprintf(
                stderr,
                "Eroare: pentru parallel2d_blocks, rows si cols trebuie sa fie divizibile cu proces grid-ul MPI %d x %d.\n",
                dims[0],
                dims[1]
            );
        }

        MPI_Abort(MPI_COMM_WORLD, 1);
    }

    MPI_Comm cart_comm;

    MPI_Cart_create(
        MPI_COMM_WORLD,
        2,
        dims,
        periods,
        0,
        &cart_comm
    );

    MPI_Cart_coords(
        cart_comm,
        rank,
        2,
        coords
    );

    int local_rows = rows / dims[0];
    int local_cols = cols / dims[1];

    int row_offset = coords[0] * local_rows;
    int col_offset = coords[1] * local_cols;

    int up, down, left, right;

    MPI_Cart_shift(cart_comm, 0, 1, &up, &down);
    MPI_Cart_shift(cart_comm, 1, 1, &left, &right);

    int local_width = local_cols + 2;

    unsigned char *current = alloc_grid(local_rows + 2, local_cols + 2);
    unsigned char *next = alloc_grid(local_rows + 2, local_cols + 2);

    if (!current || !next) {
        fprintf(stderr, "Eroare alocare memorie blocks pe rank %d.\n", rank);
        MPI_Abort(MPI_COMM_WORLD, 1);
    }

    unsigned char *global = NULL;

    if (rank == 0) {
        global = alloc_grid(rows, cols);

        if (!global) {
            fprintf(stderr, "Eroare alocare grid global blocks.\n");
            MPI_Abort(MPI_COMM_WORLD, 1);
        }

        apply_pattern(global, rows, cols, pattern_type, (unsigned int)time(NULL));

        for (int p = 0; p < size; p++) {
            int pcoords[2];

            MPI_Cart_coords(cart_comm, p, 2, pcoords);

            int p_row_offset = pcoords[0] * local_rows;
            int p_col_offset = pcoords[1] * local_cols;

            if (p == 0) {
                copy_global_to_local_block(
                    global,
                    current,
                    // rows, --- IGNORE ---
                    cols,
                    local_rows,
                    local_cols,
                    p_row_offset,
                    p_col_offset
                );
            } else {
                unsigned char *buffer = malloc(
                    (size_t)local_rows * (size_t)local_cols
                );

                if (!buffer) {
                    fprintf(stderr, "Eroare buffer scatter blocks.\n");
                    MPI_Abort(MPI_COMM_WORLD, 1);
                }

                for (int r = 0; r < local_rows; r++) {
                    for (int c = 0; c < local_cols; c++) {
                        buffer[r * local_cols + c] =
                            global[(p_row_offset + r) * cols + (p_col_offset + c)];
                    }
                }

                MPI_Send(
                    buffer,
                    local_rows * local_cols,
                    MPI_UNSIGNED_CHAR,
                    p,
                    10,
                    MPI_COMM_WORLD
                );

                free(buffer);
            }
        }
    } else {
        unsigned char *buffer = malloc(
            (size_t)local_rows * (size_t)local_cols
        );

        if (!buffer) {
            fprintf(stderr, "Eroare buffer receive blocks pe rank %d.\n", rank);
            MPI_Abort(MPI_COMM_WORLD, 1);
        }

        MPI_Recv(
            buffer,
            local_rows * local_cols,
            MPI_UNSIGNED_CHAR,
            0,
            10,
            MPI_COMM_WORLD,
            MPI_STATUS_IGNORE
        );

        for (int r = 0; r < local_rows; r++) {
            for (int c = 0; c < local_cols; c++) {
                current[idx(r + 1, c + 1, local_width)] =
                    buffer[r * local_cols + c];
            }
        }

        free(buffer);
    }

    MPI_Datatype column_type;

    MPI_Type_vector(
        local_rows,
        1,
        local_width,
        MPI_UNSIGNED_CHAR,
        &column_type
    );

    MPI_Type_commit(&column_type);

    MPI_Request requests[8];

    double start_time = MPI_Wtime();
    double communication_time = 0.0;
    double computation_time = 0.0;

    for (int step = 0; step < steps; step++) {
        memset(
            next,
            0,
            (size_t)(local_rows + 2) * (size_t)(local_cols + 2)
        );

        double comm_start = MPI_Wtime();

        MPI_Irecv(&current[idx(0, 1, local_width)], local_cols, MPI_UNSIGNED_CHAR, up, 100, cart_comm, &requests[0]);
        MPI_Irecv(&current[idx(local_rows + 1, 1, local_width)], local_cols, MPI_UNSIGNED_CHAR, down, 200, cart_comm, &requests[1]);
        MPI_Irecv(&current[idx(1, 0, local_width)], 1, column_type, left, 300, cart_comm, &requests[2]);
        MPI_Irecv(&current[idx(1, local_cols + 1, local_width)], 1, column_type, right, 400, cart_comm, &requests[3]);

        MPI_Isend(&current[idx(1, 1, local_width)], local_cols, MPI_UNSIGNED_CHAR, up, 200, cart_comm, &requests[4]);
        MPI_Isend(&current[idx(local_rows, 1, local_width)], local_cols, MPI_UNSIGNED_CHAR, down, 100, cart_comm, &requests[5]);
        MPI_Isend(&current[idx(1, 1, local_width)], 1, column_type, left, 400, cart_comm, &requests[6]);
        MPI_Isend(&current[idx(1, local_cols, local_width)], 1, column_type, right, 300, cart_comm, &requests[7]);

        communication_time += MPI_Wtime() - comm_start;

        double comp_start = MPI_Wtime();

        if (local_rows > 2 && local_cols > 2) {
            step_blocks_range(
                current,
                next,
                local_rows,
                local_cols,
                2,
                local_rows - 1
            );
        }

        computation_time += MPI_Wtime() - comp_start;

        comm_start = MPI_Wtime();

        MPI_Waitall(
            8,
            requests,
            MPI_STATUSES_IGNORE
        );

        communication_time += MPI_Wtime() - comm_start;

        comp_start = MPI_Wtime();

        if (local_rows >= 1) {
            step_blocks_range(current, next, local_rows, local_cols, 1, 1);
        }

        if (local_rows >= 2) {
            step_blocks_range(current, next, local_rows, local_cols, local_rows, local_rows);
        }

        computation_time += MPI_Wtime() - comp_start;

        unsigned char *tmp = current;
        current = next;
        next = tmp;
    }

    if (rank == 0) {
        memset(global, 0, (size_t)rows * (size_t)cols);

        copy_local_block_to_global(
            global,
            current,
            cols,
            local_rows,
            local_cols,
            row_offset,
            col_offset
        );

        for (int p = 1; p < size; p++) {
            int pcoords[2];

            MPI_Cart_coords(cart_comm, p, 2, pcoords);

            int p_row_offset = pcoords[0] * local_rows;
            int p_col_offset = pcoords[1] * local_cols;

            unsigned char *buffer = malloc(
                (size_t)local_rows * (size_t)local_cols
            );

            if (!buffer) {
                fprintf(stderr, "Eroare buffer gather blocks.\n");
                MPI_Abort(MPI_COMM_WORLD, 1);
            }

            MPI_Recv(
                buffer,
                local_rows * local_cols,
                MPI_UNSIGNED_CHAR,
                p,
                20,
                MPI_COMM_WORLD,
                MPI_STATUS_IGNORE
            );

            for (int r = 0; r < local_rows; r++) {
                for (int c = 0; c < local_cols; c++) {
                    global[(p_row_offset + r) * cols + (p_col_offset + c)] =
                        buffer[r * local_cols + c];
                }
            }

            free(buffer);
        }
    } else {
        unsigned char *buffer = malloc(
            (size_t)local_rows * (size_t)local_cols
        );

        if (!buffer) {
            fprintf(stderr, "Eroare buffer send result blocks pe rank %d.\n", rank);
            MPI_Abort(MPI_COMM_WORLD, 1);
        }

        for (int r = 0; r < local_rows; r++) {
            for (int c = 0; c < local_cols; c++) {
                buffer[r * local_cols + c] =
                    current[idx(r + 1, c + 1, local_width)];
            }
        }

        MPI_Send(
            buffer,
            local_rows * local_cols,
            MPI_UNSIGNED_CHAR,
            0,
            20,
            MPI_COMM_WORLD
        );

        free(buffer);
    }

    double elapsed = MPI_Wtime() - start_time;

    if (rank == 0) {
        if (write_output_files(out_path, global, rows, cols)) {
            double communication_percent = 0.0;

            if (elapsed > 0.0) {
                communication_percent = (communication_time / elapsed) * 100.0;
            }

            printf("Timp total simulare MPI 2D blocks: %.6f secunde\n", elapsed);
            printf("Timp petrecut in comunicatia MPI halo exchange blocks: %.6f secunde\n", communication_time);
            printf("Timp petrecut in calculul generatiilor Game of Life blocks: %.6f secunde\n", computation_time);
            printf("Procent din timpul total folosit pentru comunicatie MPI blocks: %.2f%%\n", communication_percent);

            append_benchmark_csv(
                out_path,
                "parallel2d_blocks",
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
    }

    MPI_Type_free(&column_type);
    MPI_Comm_free(&cart_comm);

    free(current);
    free(next);

    if (rank == 0) {
        free(global);
    }
}
