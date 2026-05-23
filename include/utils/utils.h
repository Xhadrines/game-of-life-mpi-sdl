#ifndef UTILS_H
#define UTILS_H

unsigned char *alloc_grid(
    int rows,
    int cols
);

void random_fill(
    unsigned char *grid,
    int rows,
    int cols,
    double alive_prob,
    unsigned int seed
);

int write_pgm(
    const char *path,
    const unsigned char *grid,
    int rows,
    int cols
);

int write_ppm(
    const char *path,
    const unsigned char *grid,
    int rows,
    int cols
);

int write_output_images(
    const char *pgm_path,
    const unsigned char *grid,
    int rows,
    int cols
);

#endif
