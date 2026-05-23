#ifndef GOL_2D_VALIDATION_H
#define GOL_2D_VALIDATION_H

/*
 * Functie: run_gol_2d_validation
 * Ce face: valideaza implementarea MPI comparativ cu rezultatul serial (bit-cu-bit).
 * Parametri: rows, cols, steps, pattern_type
 * Observasie: foloseste un seed determinist in cod pentru reproducere.
 */
void run_gol_2d_validation(
    int rows,
    int cols,
    int steps,
    int pattern_type
);

#endif
