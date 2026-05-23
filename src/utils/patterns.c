#include <stdlib.h>
#include <string.h>

#include "../../include/utils/patterns.h"
#include "../../include/utils/utils.h"

/*
 * Functie: pattern_name
 * Ce face: intoarce un nume usor de citit pentru tipul de pattern.
 * Parametri: pattern_type (PATTERN_RANDOM, PATTERN_GLIDER, PATTERN_BLINKER)
 * Returneaza: const char* cu numele pattern-ului.
 */
const char *pattern_name(int pattern_type) {
    switch (pattern_type) {
        case PATTERN_RANDOM:
            return "random";
        case PATTERN_GLIDER:
            return "glider";
        case PATTERN_BLINKER:
            return "blinker";
        default:
            return "random";
    }
}

/*
 * Functie: clear_grid (static, helper)
 * Ce face: seteaza toate celulele la 0.
 * Parametri: grid, rows, cols
 */
static void clear_grid(unsigned char *grid, int rows, int cols) {
    memset(grid, 0, (size_t)rows * (size_t)cols);
}

/*
 * Functie: set_cell (static, helper)
 * Ce face: seteaza o celula la 1 daca coordonatele sunt valide.
 * Parametri: grid, rows, cols, r, c
 */
static void set_cell(unsigned char *grid, int rows, int cols, int r, int c) {
    if (!grid) return;
    if (r < 0 || r >= rows) return;
    if (c < 0 || c >= cols) return;

    grid[r * cols + c] = 1;
}

/*
 * Functie: apply_glider (static, helper)
 * Ce face: aplica pattern-ul "glider" centrat in grid.
 */
static void apply_glider(unsigned char *grid, int rows, int cols) {
    clear_grid(grid, rows, cols);

    int r = rows / 2;
    int c = cols / 2;

    set_cell(grid, rows, cols, r,     c + 1);
    set_cell(grid, rows, cols, r + 1, c + 2);
    set_cell(grid, rows, cols, r + 2, c);
    set_cell(grid, rows, cols, r + 2, c + 1);
    set_cell(grid, rows, cols, r + 2, c + 2);
}

/*
 * Functie: apply_blinker (static, helper)
 * Ce face: aplica pattern-ul "blinker" centrat in grid.
 */
static void apply_blinker(unsigned char *grid, int rows, int cols) {
    clear_grid(grid, rows, cols);

    int r = rows / 2;
    int c = cols / 2;

    set_cell(grid, rows, cols, r, c - 1);
    set_cell(grid, rows, cols, r, c);
    set_cell(grid, rows, cols, r, c + 1);
}

/*
 * Functie: apply_pattern
 * Ce face: alege si aplica un pattern pe grid: random, glider sau blinker.
 * Parametri: grid, rows, cols, pattern_type, seed (pentru random)
 * Atensionari: Daca grid este NULL, nu face nimic.
 */
void apply_pattern(
    unsigned char *grid,
    int rows,
    int cols,
    int pattern_type,
    unsigned int seed
) {
    if (!grid) return;

    if (pattern_type == PATTERN_GLIDER) {
        apply_glider(grid, rows, cols);
    } else if (pattern_type == PATTERN_BLINKER) {
        apply_blinker(grid, rows, cols);
    } else {
        random_fill(grid, rows, cols, 0.30, seed);
    }
}
