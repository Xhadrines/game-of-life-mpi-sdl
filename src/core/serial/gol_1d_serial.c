#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "../../../include/utils/utils.h"
#include "../../../include/ui/sdl_viewer.h"
#include "../../../include/core/serial/gol_1d_serial.h"

static unsigned char rule30(
    unsigned char left,
    unsigned char center,
    unsigned char right
) {
    int pattern = (left << 2) | (center << 1) | right;

    return (30 >> pattern) & 1;
}

static void run_gol_1d_serial_internal(
    int global_n,
    int steps,
    const char *out_path,
    int use_sdl,
    int scale,
    int delay_ms
) {
    unsigned char *current = calloc((size_t)global_n + 2, sizeof(unsigned char));
    unsigned char *next = calloc((size_t)global_n + 2, sizeof(unsigned char));
    unsigned char *history = alloc_grid(steps + 1, global_n);

    if (!current || !next || !history) {
        fprintf(stderr, "Eroare alocare memorie serial 1D.\n");

        free(current);
        free(next);
        free(history);

        return;
    }

    srand((unsigned int)time(NULL));

    for (int i = 1; i <= global_n; i++) {
        current[i] = rand() % 2;
    }

    int running = 1;

    if (use_sdl) {
        running = sdl_init_viewer(steps + 1, global_n, scale);
    }

    clock_t start = clock();

    for (int step = 0; step <= steps && running; step++) {
        memcpy(
            &history[step * global_n],
            &current[1],
            (size_t)global_n
        );

        if (use_sdl) {
            running = sdl_render_grid(
                history,
                steps + 1,
                global_n,
                delay_ms,
                "SERIAL 1D",
                step,
                steps
            );
        }

        if (step == steps || !running) {
            break;
        }

        current[0] = current[global_n];
        current[global_n + 1] = current[1];

        for (int i = 1; i <= global_n; i++) {
            next[i] = rule30(
                current[i - 1],
                current[i],
                current[i + 1]
            );
        }

        unsigned char *tmp = current;
        current = next;
        next = tmp;

        memset(next, 0, (size_t)global_n + 2);
    }

    clock_t end = clock();
    double elapsed = (double)(end - start) / CLOCKS_PER_SEC;

    printf("Timp serial 1D: %.6f secunde\n", elapsed);

    if (write_pgm(out_path, history, steps + 1, global_n)) {
        printf("Imagine salvata: %s\n", out_path);
    }

    if (use_sdl) {
        sdl_wait_for_enter();
        sdl_close_viewer();
    }

    free(current);
    free(next);
    free(history);
}

void run_gol_1d_serial(
    int global_n,
    int steps,
    const char *out_path
) {
    run_gol_1d_serial_internal(
        global_n,
        steps,
        out_path,
        0,
        1,
        0
    );
}

void run_gol_1d_serial_visual(
    int global_n,
    int steps,
    const char *out_path,
    int scale,
    int delay_ms
) {
    if (scale <= 0) {
        scale = 1;
    }

    if (delay_ms < 0) {
        delay_ms = 10;
    }

    run_gol_1d_serial_internal(
        global_n,
        steps,
        out_path,
        1,
        scale,
        delay_ms
    );
}