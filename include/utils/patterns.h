#ifndef PATTERNS_H
#define PATTERNS_H

#define PATTERN_RANDOM 0
#define PATTERN_GLIDER 1
#define PATTERN_BLINKER 2

const char *pattern_name(int pattern_type);

void apply_pattern(
    unsigned char *grid,
    int rows,
    int cols,
    int pattern_type,
    unsigned int seed
);

#endif
