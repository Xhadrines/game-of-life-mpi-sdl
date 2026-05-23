#ifndef SDL_VIEWER_H
#define SDL_VIEWER_H

/*
 * Functie: sdl_app_menu
 * Ce face: afiseaza meniul interactiv pe ecran (folosit de procesul 0). Returneaza 1 daca s-a confirmat.
 * Parametri: mpi_size (nr. procese), pointeri catre variabilele care vor fi setate de UI.
 */
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

/*
 * Functie: sdl_init_viewer
 * Ce face: inisializeaza fereastra de vizualizare pentru rularea simularii.
 * Parametri: rows, cols, scale (scalare pixeli)
 * Returneaza: 1 la succes, 0 la eroare.
 */
int sdl_init_viewer(
    int rows,
    int cols,
    int scale
);

/*
 * Functie: sdl_render_grid
 * Ce face: deseneaza grid-ul curent pe ecran, trateaza input (pauza/iesire) si returneaza
 *          1 daca simularea continua sau 0 daca trebuie oprita.
 */
int sdl_render_grid(
    const unsigned char *grid,
    int rows,
    int cols,
    int delay_ms,
    const char *mode_name,
    int generation,
    int total_generations
);

/*
 * Functie: sdl_wait_for_enter
 * Ce face: afiseaza un mesaj si asteapta ENTER/ESC; folosit dupa terminarea simularii.
 */
void sdl_wait_for_enter(const char *message, double elapsed);

/*
 * Functie: sdl_close_viewer
 * Ce face: inchide fereastra si curasa resursele SDL.
 */
void sdl_close_viewer(void);

#endif
