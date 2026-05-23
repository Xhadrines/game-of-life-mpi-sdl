#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../../../include/utils/utils.h"
#include "../../../include/utils/patterns.h"
#include "../../../include/core/validation/gol_2d_validation.h"

#define VALIDATION_SEED 42

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

static int alive_at_serial(
    const unsigned char *grid,
    int rows,
    int cols,
    int r,
    int c
) {
    int wrapped_r = (r + rows) % rows;
    int wrapped_c = (c + cols) % cols;

    return grid[wrapped_r * cols + wrapped_c] ? 1 : 0;
}

static void step_serial_toroidal(
    const unsigned char *current,
    unsigned char *next,
    int rows,
    int cols
) {
    for (int r = 0; r < rows; r++) {
        for (int c = 0; c < cols; c++) {
            int neighbors = 0;

            neighbors += alive_at_serial(current, rows, cols, r - 1, c - 1);
            neighbors += alive_at_serial(current, rows, cols, r - 1, c);
            neighbors += alive_at_serial(current, rows, cols, r - 1, c + 1);

            neighbors += alive_at_serial(current, rows, cols, r, c - 1);
            neighbors += alive_at_serial(current, rows, cols, r, c + 1);

            neighbors += alive_at_serial(current, rows, cols, r + 1, c - 1);
            neighbors += alive_at_serial(current, rows, cols, r + 1, c);
            neighbors += alive_at_serial(current, rows, cols, r + 1, c + 1);

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

static int alive_at_parallel(
    const unsigned char *grid,
    int local_rows,
    int cols,
    int r,
    int c
) {
    if (r < 0 || r >= local_rows + 2) {
        return 0;
    }

    int wrapped_c = (c + cols) % cols;

    return grid[r * cols + wrapped_c] ? 1 : 0;
}

static void step_parallel_toroidal(
    const unsigned char *current,
    unsigned char *next,
    int local_rows,
    int cols
) {
    for (int r = 1; r <= local_rows; r++) {
        for (int c = 0; c < cols; c++) {
            int neighbors = 0;

            neighbors += alive_at_parallel(current, local_rows, cols, r - 1, c - 1);
            neighbors += alive_at_parallel(current, local_rows, cols, r - 1, c);
            neighbors += alive_at_parallel(current, local_rows, cols, r - 1, c + 1);

            neighbors += alive_at_parallel(current, local_rows, cols, r, c - 1);
            neighbors += alive_at_parallel(current, local_rows, cols, r, c + 1);

            neighbors += alive_at_parallel(current, local_rows, cols, r + 1, c - 1);
            neighbors += alive_at_parallel(current, local_rows, cols, r + 1, c);
            neighbors += alive_at_parallel(current, local_rows, cols, r + 1, c + 1);

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

void run_gol_2d_validation(
    int rows,
    int cols,
    int steps,
    int pattern_type
) {
    int rank, size;

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    int local_rows = owner_rows(rows, size, rank);

    unsigned char *initial = NULL;
    unsigned char *serial_current = NULL;
    unsigned char *serial_next = NULL;
    unsigned char *mpi_result = NULL;

    int *sendcounts = NULL;
    int *displs = NULL;

    if (rank == 0) {
        initial = alloc_grid(rows, cols);
        serial_current = alloc_grid(rows, cols);
        serial_next = alloc_grid(rows, cols);
        mpi_result = alloc_grid(rows, cols);

        sendcounts = malloc((size_t)size * sizeof(int));
        displs = malloc((size_t)size * sizeof(int));

        if (!initial || !serial_current || !serial_next || !mpi_result || !sendcounts || !displs) {
            fprintf(stderr, "Eroare alocare memorie validare pe rank 0.\n");
            MPI_Abort(MPI_COMM_WORLD, 1);
        }

        apply_pattern(
            initial,
            rows,
            cols,
            pattern_type,
            VALIDATION_SEED
        );

        memcpy(
            serial_current,
            initial,
            (size_t)rows * (size_t)cols
        );

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
        fprintf(stderr, "Eroare alocare memorie validare pe rank %d.\n", rank);
        MPI_Abort(MPI_COMM_WORLD, 1);
    }

    MPI_Scatterv(
        initial,
        sendcounts,
        displs,
        MPI_UNSIGNED_CHAR,
        &current[cols],
        local_rows * cols,
        MPI_UNSIGNED_CHAR,
        0,
        MPI_COMM_WORLD
    );

    if (rank == 0) {
        for (int step = 0; step < steps; step++) {
            step_serial_toroidal(
                serial_current,
                serial_next,
                rows,
                cols
            );

            unsigned char *tmp = serial_current;
            serial_current = serial_next;
            serial_next = tmp;

            memset(
                serial_next,
                0,
                (size_t)rows * (size_t)cols
            );
        }
    }

    int up = (rank - 1 + size) % size;
    int down = (rank + 1) % size;

    MPI_Request requests[4];

    for (int step = 0; step < steps; step++) {
        MPI_Irecv(
            &current[0],
            cols,
            MPI_UNSIGNED_CHAR,
            up,
            200,
            MPI_COMM_WORLD,
            &requests[0]
        );

        MPI_Irecv(
            &current[(local_rows + 1) * cols],
            cols,
            MPI_UNSIGNED_CHAR,
            down,
            100,
            MPI_COMM_WORLD,
            &requests[1]
        );

        MPI_Isend(
            &current[cols],
            cols,
            MPI_UNSIGNED_CHAR,
            up,
            100,
            MPI_COMM_WORLD,
            &requests[2]
        );

        MPI_Isend(
            &current[local_rows * cols],
            cols,
            MPI_UNSIGNED_CHAR,
            down,
            200,
            MPI_COMM_WORLD,
            &requests[3]
        );

        MPI_Waitall(
            4,
            requests,
            MPI_STATUSES_IGNORE
        );

        memset(
            next,
            0,
            (size_t)(local_rows + 2) * (size_t)cols
        );

        step_parallel_toroidal(
            current,
            next,
            local_rows,
            cols
        );

        unsigned char *tmp = current;
        current = next;
        next = tmp;
    }

    MPI_Gatherv(
        &current[cols],
        local_rows * cols,
        MPI_UNSIGNED_CHAR,
        mpi_result,
        sendcounts,
        displs,
        MPI_UNSIGNED_CHAR,
        0,
        MPI_COMM_WORLD
    );

    if (rank == 0) {
        int valid = 1;
        int diff_r = -1;
        int diff_c = -1;

        for (int i = 0; i < rows * cols; i++) {
            if (serial_current[i] != mpi_result[i]) {
                diff_r = i / cols;
                diff_c = i % cols;

                valid = 0;
                break;
            }
        }

        write_output_files(
            "validation_serial",
            serial_current,
            rows,
            cols
        );

        write_output_files(
            "validation_mpi",
            mpi_result,
            rows,
            cols
        );

        if (valid) {
            printf("VALIDATION PASSED: rezultatul MPI este identic bit-cu-bit cu rezultatul serial.\n");
        } else {
            printf("VALIDATION FAILED: rezultatul MPI difera de rezultatul serial.\n");
            printf(
                "Prima diferenta gasita la celula (%d, %d): serial=%d mpi=%d\n",
                diff_r,
                diff_c,
                serial_current[diff_r * cols + diff_c],
                mpi_result[diff_r * cols + diff_c]
            );
        }

        printf(
            "Grid validare: %d x %d\n",
            rows,
            cols
        );

        printf(
            "Generatii validate: %d\n",
            steps
        );

        printf(
            "Procese MPI folosite: %d\n",
            size
        );

        printf(
            "Seed determinist: %d\n",
            VALIDATION_SEED
        );

        printf("Imagine PGM serial salvata: output/pgm/validation_serial.pgm\n");
        printf("Imagine PPM serial salvata: output/ppm/validation_serial.ppm\n");

        if (rows <= TXT_EXPORT_MAX_ROWS && cols <= TXT_EXPORT_MAX_COLS) {
            printf("Fisier TXT serial salvat: output/txt/validation_serial.txt\n");
        }

        printf("Imagine PGM MPI salvata: output/pgm/validation_mpi.pgm\n");
        printf("Imagine PPM MPI salvata: output/ppm/validation_mpi.ppm\n");

        if (rows <= TXT_EXPORT_MAX_ROWS && cols <= TXT_EXPORT_MAX_COLS) {
            printf("Fisier TXT MPI salvat: output/txt/validation_mpi.txt\n");
        }
    }

    free(current);
    free(next);

    if (rank == 0) {
        free(initial);
        free(serial_current);
        free(serial_next);
        free(mpi_result);
        free(sendcounts);
        free(displs);
    }
}
