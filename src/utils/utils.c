#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>

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
    
    mkdir("output", 0777);

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