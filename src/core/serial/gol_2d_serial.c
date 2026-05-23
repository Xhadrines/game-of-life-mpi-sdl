#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "../../../include/utils/utils.h"
#include "../../../include/utils/patterns.h"
#include "../../../include/ui/sdl_viewer.h"
#include "../../../include/core/serial/gol_2d_serial.h"

/*
 * Functie: alive_at (static)
 * Ce face: intoarce 1 daca celula (r,c) este vie, cu wrap-around toroidal.
 */
static int alive_at(
    const unsigned char *grid,
    int rows,
    int cols,
    int r,
    int c
) {
    int wrapped_r = (r + rows) % rows;
    int wrapped_c = (c + cols) % cols;

    return grid[wrapped_r * cols + wrapped_c] ? 1 : 0;
}

/*
 * Functie: step_2d_serial (static)
 * Ce face: calculeaza next pentru fiecare celula din grid-ul serial (toroidal).
 */
static void step_2d_serial(
    const unsigned char *current,
    unsigned char *next,
    int rows,
    int cols
) {
    for (int r = 0; r < rows; r++) {
        for (int c = 0; c < cols; c++) {
            int neighbors = 0;

            neighbors += alive_at(current, rows, cols, r - 1, c - 1);
            neighbors += alive_at(current, rows, cols, r - 1, c);
            neighbors += alive_at(current, rows, cols, r - 1, c + 1);

            neighbors += alive_at(current, rows, cols, r, c - 1);
            neighbors += alive_at(current, rows, cols, r, c + 1);

            neighbors += alive_at(current, rows, cols, r + 1, c - 1);
            neighbors += alive_at(current, rows, cols, r + 1, c);
            neighbors += alive_at(current, rows, cols, r + 1, c + 1);

            unsigned char cell = current[r * cols + c];

            if (cell && (neighbors == 2 || neighbors == 3)) {
                next[r * cols + c] = 1;
            } else if (!cell && neighbors == 3) {
                next[r * cols + c] = 1;
            } else {
                next[r * cols + c] = 0;
            }
        }
    }
}

/*
 * Functie: run_gol_2d_serial_internal (static)
 * Ce face: implementeaza bucla principala pentru simularea seriala 2D;
 *          poate rula cu sau fara UI (use_sdl).
 * Parametri: rows, cols, steps, out_path, use_sdl (0/1), scale, delay_ms, pattern_type
 */
static void run_gol_2d_serial_internal(
    int rows,
    int cols,
    int steps,
    const char *out_path,
    int use_sdl,
    int scale,
    int delay_ms,
    int pattern_type
) {
    unsigned char *current = alloc_grid(rows, cols);
    unsigned char *next = alloc_grid(rows, cols);

    if (!current || !next) {
        fprintf(stderr, "Eroare alocare memorie serial 2D.\n");

        free(current);
        free(next);

        return;
    }

    apply_pattern(current, rows, cols, pattern_type, (unsigned int)time(NULL));

    int running = 1;

    if (use_sdl) {
        running = sdl_init_viewer(rows, cols, scale);
    }

    clock_t start = clock();

    if (use_sdl) {
        running = sdl_render_grid(
            current,
            rows,
            cols,
            delay_ms,
            "SERIAL 2D",
            0,
            steps
        );
    }

    for (int step = 1; step <= steps && running; step++) {
        step_2d_serial(
            current,
            next,
            rows,
            cols
        );

        unsigned char *tmp = current;
        current = next;
        next = tmp;

        memset(
            next,
            0,
            (size_t)rows * (size_t)cols
        );

        if (use_sdl) {
            running = sdl_render_grid(
                current,
                rows,
                cols,
                delay_ms,
                "SERIAL 2D",
                step,
                steps
            );
        }
    }

    clock_t end = clock();
    double elapsed = (double)(end - start) / CLOCKS_PER_SEC;

    printf("Timp serial 2D: %.6f secunde\n", elapsed);

    if (write_output_files(out_path, current, rows, cols)) {
        printf("Imagine PGM salvata: output/pgm/%s.pgm\n", out_path);
        printf("Imagine PPM salvata: output/ppm/%s.ppm\n", out_path);

        if (rows <= TXT_EXPORT_MAX_ROWS && cols <= TXT_EXPORT_MAX_COLS) {
            printf("Fisier TXT salvat: output/txt/%s.txt\n", out_path);
        }
    }

    if (use_sdl) {
        sdl_wait_for_enter("SIMULARE SERIALA FINALIZATA", elapsed);
        sdl_close_viewer();
    }

    free(current);
    free(next);
}

/*
 * Functie: run_gol_2d_serial
 * Ce face: wrapper pentru rulare non-vizuala a simularii seriale 2D.
 */
void run_gol_2d_serial(
    int rows,
    int cols,
    int steps,
    const char *out_path,
    int pattern_type
) {
    run_gol_2d_serial_internal(
        rows,
        cols,
        steps,
        out_path,
        0,
        1,
        0,
        pattern_type
    );
}

/*
 * Functie: run_gol_2d_serial_visual
 * Ce face: wrapper pentru rulare cu vizualizare SDL2 a simularii seriale 2D.
 */
void run_gol_2d_serial_visual(
    int rows,
    int cols,
    int steps,
    const char *out_path,
    int scale,
    int delay_ms,
    int pattern_type
) {
    if (scale <= 0) {
        scale = 2;
    }

    if (delay_ms < 0) {
        delay_ms = 10;
    }

    run_gol_2d_serial_internal(
        rows,
        cols,
        steps,
        out_path,
        1,
        scale,
        delay_ms,
        pattern_type
    );
}
