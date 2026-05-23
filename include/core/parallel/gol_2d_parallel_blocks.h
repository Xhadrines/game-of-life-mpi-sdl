#ifndef GOL_2D_PARALLEL_BLOCKS_H
#define GOL_2D_PARALLEL_BLOCKS_H

/*
 * Functie: run_gol_2d_parallel_blocks
 * Ce face: ruleaza decompozitia 2D (blocks) pe o topologie carteziana de procese.
 * Observasie: rows si cols trebuie sa fie divizibile cu dimensiunile grid-ului de procese.
 */
void run_gol_2d_parallel_blocks(
    int rows,
    int cols,
    int steps,
    const char *out_path,
    int pattern_type
);

#endif
