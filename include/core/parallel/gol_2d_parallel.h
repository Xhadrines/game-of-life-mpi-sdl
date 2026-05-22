#ifndef GOL_2D_PARALLEL_H
#define GOL_2D_PARALLEL_H

void run_gol_2d_parallel(
    int rows,
    int cols,
    int steps,
    const char *out_path
);

void run_gol_2d_parallel_visual(
    int rows,
    int cols,
    int steps,
    const char *out_path,
    int scale,
    int delay_ms
);

#endif
