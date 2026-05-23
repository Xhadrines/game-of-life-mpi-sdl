#ifndef GOL_1D_SERIAL_H
#define GOL_1D_SERIAL_H

/*
 * Functie: run_gol_1d_serial
 * Ce face: ruleaza implementarea seriala 1D (rule30) si exporta rezultatul.
 */
void run_gol_1d_serial(
    int global_n,
    int steps,
    const char *out_path
);

/*
 * Functie: run_gol_1d_serial_visual
 * Ce face: versiunea cu vizualizare (afiseaza istoricul in SDL2).
 */
void run_gol_1d_serial_visual(
    int global_n,
    int steps,
    const char *out_path,
    int scale,
    int delay_ms
);

#endif
