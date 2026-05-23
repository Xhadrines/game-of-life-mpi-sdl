#ifndef GOL_2D_PARALLEL_TOROIDAL_H
#define GOL_2D_PARALLEL_TOROIDAL_H

/*
 * Functie: run_gol_2d_parallel_toroidal
 * Ce face: ruleaza MPI 2D cu topologie toroidala (wrap pe marginile randurilor si coloanelor).
 */
void run_gol_2d_parallel_toroidal(
    int rows,
    int cols,
    int steps,
    const char *out_path,
    int pattern_type
);

/*
 * Functie: run_gol_2d_parallel_toroidal_visual
 * Ce face: versiunea vizuala a simularii toroidale (rank 0 afiseaza cu SDL2).
 */
void run_gol_2d_parallel_toroidal_visual(
    int rows,
    int cols,
    int steps,
    const char *out_path,
    int scale,
    int delay_ms,
    int pattern_type
);

#endif
