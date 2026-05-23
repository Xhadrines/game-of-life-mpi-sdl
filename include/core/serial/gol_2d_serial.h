#ifndef GOL_2D_SERIAL_H
#define GOL_2D_SERIAL_H

void run_gol_2d_serial(
    int rows,
    int cols,
    int steps,
    const char *out_path,
    int pattern_type
);

void run_gol_2d_serial_visual(
    int rows,
    int cols,
    int steps,
    const char *out_path,
    int scale,
    int delay_ms,
    int pattern_type
);

#endif
