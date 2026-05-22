#ifndef GOL_1D_H
#define GOL_1D_H

void run_gol_1d(
    int global_n,
    int steps,
    const char *out_path
);

void run_gol_1d_visual(
    int global_n,
    int steps,
    const char *out_path,
    int scale,
    int delay_ms
);

#endif
