#include <stdlib.h>
#include <string.h>

#include "../../include/utils/patterns.h"
#include "../../include/utils/utils.h"

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

static void clear_grid(unsigned char *grid, int rows, int cols) {
    memset(grid, 0, (size_t)rows * (size_t)cols);
}

static void set_cell(unsigned char *grid, int rows, int cols, int r, int c) {
    if (!grid) return;
    if (r < 0 || r >= rows) return;
    if (c < 0 || c >= cols) return;

    grid[r * cols + c] = 1;
}

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

static void apply_blinker(unsigned char *grid, int rows, int cols) {
    clear_grid(grid, rows, cols);

    int r = rows / 2;
    int c = cols / 2;

    set_cell(grid, rows, cols, r, c - 1);
    set_cell(grid, rows, cols, r, c);
    set_cell(grid, rows, cols, r, c + 1);
}

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
