#ifndef GOL_1D_PARALLEL_H
#define GOL_1D_PARALLEL_H

/*
 * Functie: run_gol_1d_parallel
 * Ce face: ruleaza versiunea paralela 1D (fara UI). Util pentru benchmark-uri.
 */
void run_gol_1d_parallel(
    int global_n,
    int steps,
    const char *out_path
);

/*
 * Functie: run_gol_1d_parallel_visual
 * Ce face: wrapper pentru rulare cu vizualizare SDL2 (rank 0 afiseaza istoricul).
 */
void run_gol_1d_parallel_visual(
    int global_n,
    int steps,
    const char *out_path,
    int scale,
    int delay_ms
);

#endif
