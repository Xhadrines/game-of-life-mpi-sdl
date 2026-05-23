#ifndef PATTERNS_H
#define PATTERNS_H

#define PATTERN_RANDOM 0
#define PATTERN_GLIDER 1
#define PATTERN_BLINKER 2

/*
 * Functie: pattern_name
 * Ce face: returneaza un nume lizibil pentru tipul de pattern (random/glider/blinker).
 * Parametru: pattern_type - constanta definita (PATTERN_RANDOM etc.).
 */
const char *pattern_name(int pattern_type);

/*
 * Functie: apply_pattern
 * Ce face: aplica un pattern pe grid. Pentru random foloseste seed pentru reproducere.
 * Parametri:
 *   - grid: buffer alocat (rows*cols)
 *   - rows, cols: dimensiuni
 *   - pattern_type: una din PATTERN_RANDOM, PATTERN_GLIDER, PATTERN_BLINKER
 *   - seed: folosit doar pentru random
 * Atensionare: Nu verifica grid==NULL in semnatura; verifica in implementare.
 */
void apply_pattern(
    unsigned char *grid,
    int rows,
    int cols,
    int pattern_type,
    unsigned int seed
);

#endif
