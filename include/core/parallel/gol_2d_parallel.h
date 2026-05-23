#ifndef GOL_2D_PARALLEL_H
#define GOL_2D_PARALLEL_H

/*
 * Functie: run_gol_2d_parallel
 * Ce face: ruleaza versiunea MPI 2D (fara UI). Resine: trebuie apelat cu mpirun.
 * Parametri: rows, cols, steps, out_path (prefix pentru fisiere), pattern_type
 */
void run_gol_2d_parallel(
    int rows,
    int cols,
    int steps,
    const char *out_path,
    int pattern_type
);

/*
 * Functie: run_gol_2d_parallel_visual
 * Ce face: aceleasi ca run_gol_2d_parallel, dar afiseaza vizual cu SDL2 pe rank 0.
 * Parametri adisionali: scale (marimea pixelilor), delay_ms (intarziere intre frame-uri)
 */
void run_gol_2d_parallel_visual(
    int rows,
    int cols,
    int steps,
    const char *out_path,
    int scale,
    int delay_ms,
    int pattern_type
);

#endif
