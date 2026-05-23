#ifndef UTILS_H
#define UTILS_H

#define TXT_EXPORT_MAX_ROWS 500
#define TXT_EXPORT_MAX_COLS 500

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

int write_txt(
    const char *path,
    const unsigned char *grid,
    int rows,
    int cols
);

int write_output_files(
    const char *base_name,
    const unsigned char *grid,
    int rows,
    int cols
);

#endif
