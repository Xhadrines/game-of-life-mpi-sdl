#ifndef SDL_VIEWER_H
#define SDL_VIEWER_H

int sdl_app_menu(
    int mpi_size,
    int *mode,
    int *rows,
    int *cols,
    int *steps,
    int *scale,
    int *delay_ms,
    int *pattern_type
);

int sdl_init_viewer(
    int rows,
    int cols,
    int scale
);

int sdl_render_grid(
    const unsigned char *grid,
    int rows,
    int cols,
    int delay_ms,
    const char *mode_name,
    int generation,
    int total_generations
);

void sdl_wait_for_enter(const char *message, double elapsed);

void sdl_close_viewer(void);

#endif
