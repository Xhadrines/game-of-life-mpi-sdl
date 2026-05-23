#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../include/utils/utils.h"

unsigned char *alloc_grid(int rows, int cols) {
    if (rows <= 0 || cols <= 0) return NULL;

    return calloc((size_t)rows * (size_t)cols, sizeof(unsigned char));
}

void random_fill(unsigned char *grid, int rows, int cols, double alive_prob, unsigned int seed) {
    if (!grid) return;

    srand(seed);

    for (int i = 0; i < rows * cols; i++) {
        double r = (double)rand() / (double)RAND_MAX;
        grid[i] = r < alive_prob ? 1 : 0;
    }
}

int write_pgm(const char *path, const unsigned char *grid, int rows, int cols) {
    FILE *f = fopen(path, "w");

    if (!f) {
        perror("fopen");
        return 0;
    }

    fprintf(f, "P2\n%d %d\n255\n", cols, rows);

    for (int r = 0; r < rows; r++) {
        for (int c = 0; c < cols; c++) {
            fprintf(f, "%d ", grid[r * cols + c] ? 255 : 0);
        }

        fprintf(f, "\n");
    }

    fclose(f);
    return 1;
}

int write_ppm(const char *path, const unsigned char *grid, int rows, int cols) {
    FILE *f = fopen(path, "w");

    if (!f) {
        perror("fopen");
        return 0;
    }

    fprintf(f, "P3\n%d %d\n255\n", cols, rows);

    for (int r = 0; r < rows; r++) {
        for (int c = 0; c < cols; c++) {
            if (grid[r * cols + c]) {
                fprintf(f, "255 255 255 ");
            } else {
                fprintf(f, "0 0 0 ");
            }
        }

        fprintf(f, "\n");
    }

    fclose(f);
    return 1;
}

int write_txt(
    const char *path,
    const unsigned char *grid,
    int rows,
    int cols
) {
    FILE *f = fopen(path, "w");

    if (!f) {
        perror("fopen");
        return 0;
    }

    for (int r = 0; r < rows; r++) {
        for (int c = 0; c < cols; c++) {
            fprintf(
                f,
                "%c ",
                grid[r * cols + c] ? '#' : '.'
            );
        }

        fprintf(f, "\n");
    }

    fclose(f);
    return 1;
}

int write_output_files(
    const char *base_name,
    const unsigned char *grid,
    int rows,
    int cols
) {
    char pgm_full[512];
    char ppm_full[512];
    char txt_full[512];

    snprintf(
        pgm_full,
        sizeof(pgm_full),
        "output/pgm/%s.pgm",
        base_name
    );

    snprintf(
        ppm_full,
        sizeof(ppm_full),
        "output/ppm/%s.ppm",
        base_name
    );

    snprintf(
        txt_full,
        sizeof(txt_full),
        "output/txt/%s.txt",
        base_name
    );

    int ok_pgm = write_pgm(pgm_full, grid, rows, cols);
    int ok_ppm = write_ppm(ppm_full, grid, rows, cols);

    int ok_txt = 1;

    if (
        rows <= TXT_EXPORT_MAX_ROWS &&
        cols <= TXT_EXPORT_MAX_COLS
    ) {
        ok_txt = write_txt(
            txt_full,
            grid,
            rows,
            cols
        );
    }

    return ok_pgm && ok_ppm && ok_txt;
}

int append_benchmark_csv(
    const char *run_name,
    const char *mode,
    int processes,
    int rows,
    int cols,
    int steps,
    double total_time,
    double communication_time,
    double computation_time
) {
    FILE *check = fopen("output/benchmarks/benchmark_results.csv", "r");

    int file_exists = check != NULL;

    if (check) {
        fclose(check);
    }

    FILE *f = fopen("output/benchmarks/benchmark_results.csv", "a");

    if (!f) {
        perror("fopen");
        return 0;
    }

    if (!file_exists) {
        fprintf(f, "run_name,mode,processes,rows,cols,steps,total_time,communication_time,computation_time,communication_percent\n");
        
    }

    double communication_percent = 0.0;

    if (total_time > 0.0) {
        communication_percent =
            (communication_time / total_time) * 100.0;
    }

    fprintf(
        f,
        "%s,%s,%d,%d,%d,%d,%.6f,%.6f,%.6f,%.2f\n",
        run_name,
        mode,
        processes,
        rows,
        cols,
        steps,
        total_time,
        communication_time,
        computation_time,
        communication_percent
    );

    fclose(f);

    return 1;
}
