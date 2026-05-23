#ifndef GOL_2D_SERIAL_H
#define GOL_2D_SERIAL_H

/*
 * Functie: run_gol_2d_serial
 * Ce face: ruleaza versiunea seriala 2D (toroidala) fara UI.
 */
void run_gol_2d_serial(
    int rows,
    int cols,
    int steps,
    const char *out_path,
    int pattern_type
);

/*
 * Functie: run_gol_2d_serial_visual
 * Ce face: versiunea cu UI (SDL2) a simularii seriale 2D.
 */
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
