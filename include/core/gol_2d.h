#ifndef GOL_2D_H
#define GOL_2D_H

void run_gol_2d(
    int rows,
    int cols,
    int steps,
    const char *out_path
);

void run_gol_2d_visual(
    int rows,
    int cols,
    int steps,
    const char *out_path,
    int scale,
    int delay_ms
);

#endif
