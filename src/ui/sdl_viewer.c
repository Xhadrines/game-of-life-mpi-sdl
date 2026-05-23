#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../include/ui/sdl_viewer.h"
#include "../include/utils/patterns.h"

static SDL_Window *window = NULL;
static SDL_Renderer *renderer = NULL;
static TTF_Font *font_small = NULL;
static TTF_Font *font_medium = NULL;
static TTF_Font *font_large = NULL;

static int scale_global = 4;

static int is_1d_mode(int mode) {
    return mode == 1 || mode == 3;
}

static int is_valid_config(int mpi_size, int mode, int rows, int cols) {
    if (is_1d_mode(mode)) {
        return rows % mpi_size == 0;
    }

    return rows % mpi_size == 0 && cols % mpi_size == 0;
}

static void fix_invalid_value(int mpi_size, int mode, int *rows, int *cols) {
    if (is_1d_mode(mode)) {
        while (*rows % mpi_size != 0) {
            (*rows)++;
        }
    } else {
        while (*rows % mpi_size != 0) {
            (*rows)++;
        }

        while (*cols % mpi_size != 0) {
            (*cols)++;
        }
    }
}

static TTF_Font *load_font(int size) {
    return TTF_OpenFont("assets/fonts/DejaVuSans.ttf", size);
}

static int init_window(const char *title, int width, int height) {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        fprintf(stderr, "SDL_Init error: %s\n", SDL_GetError());
        return 0;
    }

    if (TTF_Init() != 0) {
        fprintf(stderr, "TTF_Init error: %s\n", TTF_GetError());
        SDL_Quit();
        return 0;
    }

    font_small = load_font(24);
    font_medium = load_font(32);
    font_large = load_font(54);

    if (!font_small || !font_medium || !font_large) {
        fprintf(stderr, "Nu am gasit font. Verifica assets/fonts/DejaVuSans.ttf\n");
    }

    window = SDL_CreateWindow(
        title,
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        width,
        height,
        SDL_WINDOW_SHOWN
    );

    if (!window) {
        fprintf(stderr, "SDL_CreateWindow error: %s\n", SDL_GetError());
        SDL_Quit();
        return 0;
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    if (!renderer) {
        fprintf(stderr, "SDL_CreateRenderer error: %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 0;
    }

    return 1;
}

static void close_window(void) {
    if (font_small) TTF_CloseFont(font_small);
    if (font_medium) TTF_CloseFont(font_medium);
    if (font_large) TTF_CloseFont(font_large);

    font_small = NULL;
    font_medium = NULL;
    font_large = NULL;

    if (renderer) SDL_DestroyRenderer(renderer);
    if (window) SDL_DestroyWindow(window);

    renderer = NULL;
    window = NULL;

    TTF_Quit();
    SDL_Quit();
}

static int inside(SDL_Rect rect, int x, int y) {
    return x >= rect.x &&
           x <= rect.x + rect.w &&
           y >= rect.y &&
           y <= rect.y + rect.h;
}

static void draw_text(const char *text, int x, int y, TTF_Font *font) {
    if (!font || !renderer || !text) return;

    SDL_Color color = {255, 255, 255, 255};

    SDL_Surface *surface = TTF_RenderText_Blended(font, text, color);
    if (!surface) return;

    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_Rect dst = {x, y, surface->w, surface->h};

    SDL_FreeSurface(surface);

    if (!texture) return;

    SDL_RenderCopy(renderer, texture, NULL, &dst);
    SDL_DestroyTexture(texture);
}

static void draw_text_color(
    const char *text,
    int x,
    int y,
    TTF_Font *font,
    SDL_Color color
) {
    if (!font || !renderer || !text) return;

    SDL_Surface *surface = TTF_RenderText_Blended(font, text, color);
    if (!surface) return;

    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_Rect dst = {x, y, surface->w, surface->h};

    SDL_FreeSurface(surface);

    if (!texture) return;

    SDL_RenderCopy(renderer, texture, NULL, &dst);
    SDL_DestroyTexture(texture);
}

static void draw_centered_text(const char *text, SDL_Rect rect, TTF_Font *font) {
    if (!font || !text) return;

    int w = 0;
    int h = 0;

    TTF_SizeText(font, text, &w, &h);

    int x = rect.x + (rect.w - w) / 2;
    int y = rect.y + (rect.h - h) / 2;

    draw_text(text, x, y, font);
}

static void draw_button(SDL_Rect rect, int active, int r, int g, int b, const char *label) {
    if (active) {
        SDL_SetRenderDrawColor(renderer, 230, 180, 40, 255);
    } else {
        SDL_SetRenderDrawColor(renderer, r, g, b, 255);
    }

    SDL_RenderFillRect(renderer, &rect);

    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderDrawRect(renderer, &rect);

    draw_centered_text(label, rect, font_medium);
}

static void draw_field(SDL_Rect rect, int active, const char *name, int value) {
    char buffer[64];

    if (active) {
        SDL_SetRenderDrawColor(renderer, 70, 130, 180, 255);
    } else {
        SDL_SetRenderDrawColor(renderer, 70, 70, 70, 255);
    }

    SDL_RenderFillRect(renderer, &rect);

    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderDrawRect(renderer, &rect);

    draw_text(name, rect.x + 20, rect.y + 12, font_small);

    snprintf(buffer, sizeof(buffer), "%d", value);
    draw_text(buffer, rect.x + 20, rect.y + 50, font_medium);
}

static void draw_field_text(SDL_Rect rect, int active, const char *name, const char *value) {
    if (active) {
        SDL_SetRenderDrawColor(renderer, 70, 130, 180, 255);
    } else {
        SDL_SetRenderDrawColor(renderer, 70, 70, 70, 255);
    }

    SDL_RenderFillRect(renderer, &rect);

    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderDrawRect(renderer, &rect);

    draw_text(name, rect.x + 20, rect.y + 12, font_small);
    draw_text(value, rect.x + 20, rect.y + 50, font_medium);
}

static void set_defaults_1d(int mpi_size, int *rows, int *cols, int *steps, int *scale, int *delay_ms) {
    *rows = 500 * mpi_size;
    *cols = 1;
    *steps = 500 * mpi_size;
    *scale = 1;
    *delay_ms = 10;
}

static void set_defaults_2d(int mpi_size, int *rows, int *cols, int *steps, int *scale, int *delay_ms) {
    *rows = 125 * mpi_size;
    *cols = 125 * mpi_size;
    *steps = 500 * mpi_size;
    *scale = 2;
    *delay_ms = 10;
}

int sdl_app_menu(
    int mpi_size,
    int *mode,
    int *rows,
    int *cols,
    int *steps,
    int *scale,
    int *delay_ms,
    int *pattern_type
) {
    if (!init_window("Game of Life MPI", 1920, 1080)) {
        return 0;
    }

    *mode = 2;

    SDL_Rect button_mpi_1d = {120, 170, 300, 100};
    SDL_Rect button_mpi_2d = {450, 170, 300, 100};
    SDL_Rect button_serial_1d = {780, 170, 300, 100};
    SDL_Rect button_serial_2d = {1110, 170, 300, 100};
    SDL_Rect button_toroidal = {1440, 170, 360, 100};

    SDL_Rect field_1 = {120, 360, 250, 110};
    SDL_Rect field_2 = {410, 360, 250, 110};
    SDL_Rect field_3 = {700, 360, 250, 110};
    SDL_Rect field_4 = {990, 360, 250, 110};
    SDL_Rect field_5 = {1280, 360, 250, 110};
    SDL_Rect field_6 = {1570, 360, 250, 110};

    SDL_Rect run_button = {690, 650, 540, 110};

    int field = 0;
    int running = 1;
    int confirmed = 0;

    fix_invalid_value(mpi_size, *mode, rows, cols);

    while (running) {
        int total_fields = is_1d_mode(*mode) ? 4 : 6;
        int valid = is_valid_config(mpi_size, *mode, *rows, *cols);

        if (field >= total_fields) {
            field = total_fields - 1;
        }

        SDL_Event event;

        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = 0;
            }

            if (event.type == SDL_MOUSEBUTTONDOWN) {
                int x = event.button.x;
                int y = event.button.y;

                if (inside(button_mpi_1d, x, y)) {
                    *mode = 1;
                    set_defaults_1d(mpi_size, rows, cols, steps, scale, delay_ms);
                    field = 0;
                    fix_invalid_value(mpi_size, *mode, rows, cols);
                }

                if (inside(button_mpi_2d, x, y)) {
                    *mode = 2;
                    set_defaults_2d(mpi_size, rows, cols, steps, scale, delay_ms);
                    field = 0;
                    fix_invalid_value(mpi_size, *mode, rows, cols);
                }

                if (inside(button_serial_1d, x, y)) {
                    *mode = 3;
                    set_defaults_1d(mpi_size, rows, cols, steps, scale, delay_ms);
                    field = 0;
                    fix_invalid_value(mpi_size, *mode, rows, cols);
                }

                if (inside(button_serial_2d, x, y)) {
                    *mode = 4;
                    set_defaults_2d(mpi_size, rows, cols, steps, scale, delay_ms);
                    field = 0;
                    fix_invalid_value(mpi_size, *mode, rows, cols);
                }

                if (inside(button_toroidal, x, y)) {
                    *mode = 5;
                    set_defaults_2d(mpi_size, rows, cols, steps, scale, delay_ms);
                    field = 0;
                    fix_invalid_value(mpi_size, *mode, rows, cols);
                }

                if (inside(field_1, x, y)) field = 0;
                if (inside(field_2, x, y)) field = 1;
                if (inside(field_3, x, y)) field = 2;
                if (inside(field_4, x, y)) field = 3;
                if (!is_1d_mode(*mode) && inside(field_5, x, y)) field = 4;
                if (!is_1d_mode(*mode) && inside(field_6, x, y)) field = 5;

                if (inside(run_button, x, y)) {
                    if (is_valid_config(mpi_size, *mode, *rows, *cols)) {
                        confirmed = 1;
                        running = 0;
                    }
                }
            }

            if (event.type == SDL_KEYDOWN) {
                SDL_Keycode key = event.key.keysym.sym;

                int fast = (event.key.keysym.mod & KMOD_SHIFT) ? 10 : 1;

                if (key == SDLK_ESCAPE) {
                    running = 0;
                }

                else if (key == SDLK_RETURN) {
                    if (is_valid_config(mpi_size, *mode, *rows, *cols)) {
                        confirmed = 1;
                        running = 0;
                    }
                }

                else if (key == SDLK_1) {
                    *mode = 1;
                    set_defaults_1d(mpi_size, rows, cols, steps, scale, delay_ms);
                    field = 0;
                    fix_invalid_value(mpi_size, *mode, rows, cols);
                }

                else if (key == SDLK_2) {
                    *mode = 2;
                    set_defaults_2d(mpi_size, rows, cols, steps, scale, delay_ms);
                    field = 0;
                    fix_invalid_value(mpi_size, *mode, rows, cols);
                }

                else if (key == SDLK_3) {
                    *mode = 3;
                    set_defaults_1d(mpi_size, rows, cols, steps, scale, delay_ms);
                    field = 0;
                    fix_invalid_value(mpi_size, *mode, rows, cols);
                }

                else if (key == SDLK_4) {
                    *mode = 4;
                    set_defaults_2d(mpi_size, rows, cols, steps, scale, delay_ms);
                    field = 0;
                    fix_invalid_value(mpi_size, *mode, rows, cols);
                }

                else if (key == SDLK_5) {
                    *mode = 5;
                    set_defaults_2d(mpi_size, rows, cols, steps, scale, delay_ms);
                    field = 0;
                    fix_invalid_value(mpi_size, *mode, rows, cols);
                }

                else if (key == SDLK_RIGHT) {
                    field++;
                    if (field >= total_fields) field = 0;
                }

                else if (key == SDLK_LEFT) {
                    field--;
                    if (field < 0) field = total_fields - 1;
                }

                else if (key == SDLK_UP) {
                    if (is_1d_mode(*mode)) {
                        if (field == 0) *rows += mpi_size * fast;
                        if (field == 1) *steps += 100 * fast;
                        if (field == 2) *scale += 1 * fast;
                        if (field == 3) *delay_ms += 1 * fast;
                    } else {
                        if (field == 0) *rows += mpi_size * fast;
                        if (field == 1) *cols += mpi_size * fast;
                        if (field == 2) *steps += 100 * fast;
                        if (field == 3) *scale += 1 * fast;
                        if (field == 4) *delay_ms += 1 * fast;
                        if (field == 5) *pattern_type = (*pattern_type + 1) % 3;
                    }

                    fix_invalid_value(mpi_size, *mode, rows, cols);
                }

                else if (key == SDLK_DOWN) {
                    if (is_1d_mode(*mode)) {
                        if (field == 0 && *rows > mpi_size) *rows -= mpi_size * fast;
                        if (field == 1 && *steps > 100) *steps -= 100 * fast;
                        if (field == 2 && *scale > 1) *scale -= 1 * fast;
                        if (field == 3 && *delay_ms > 0) *delay_ms -= 1 * fast;
                    } else {
                        if (field == 0 && *rows > mpi_size) *rows -= mpi_size * fast;
                        if (field == 1 && *cols > mpi_size) *cols -= mpi_size * fast;
                        if (field == 2 && *steps > 100) *steps -= 100 * fast;
                        if (field == 3 && *scale > 1) *scale -= 1 * fast;
                        if (field == 4 && *delay_ms > 0) *delay_ms -= 1 * fast;
                        if (field == 5) {
                            (*pattern_type)--;
                            if (*pattern_type < 0) *pattern_type = 2;
                        }
                    }

                    fix_invalid_value(mpi_size, *mode, rows, cols);

                    if (*rows < mpi_size) *rows = mpi_size;
                    if (*cols < mpi_size) *cols = mpi_size;
                    if (*steps < 100) *steps = 100;
                    if (*scale < 1) *scale = 1;
                    if (*delay_ms < 0) *delay_ms = 0;
                }

                else if (key == SDLK_r) {
                    if (is_1d_mode(*mode)) {
                        set_defaults_1d(mpi_size, rows, cols, steps, scale, delay_ms);
                    } else {
                        set_defaults_2d(mpi_size, rows, cols, steps, scale, delay_ms);
                    }

                    fix_invalid_value(mpi_size, *mode, rows, cols);
                }
            }
        }

        valid = is_valid_config(mpi_size, *mode, *rows, *cols);

        SDL_SetRenderDrawColor(renderer, 25, 25, 25, 255);
        SDL_RenderClear(renderer);

        SDL_Rect title_rect = {0, 20, 1920, 60};

        draw_centered_text(
            "Conway Game of Life",
            title_rect,
            font_large
        );

        SDL_Rect subtitle_rect = {0, 100, 1920, 40};

        draw_centered_text(
            "Alege modul folosind tastele 1-5",
            subtitle_rect,
            font_small
        );

        draw_button(button_mpi_1d, *mode == 1, 70, 130, 180, "1 - MPI 1D");
        draw_button(button_mpi_2d, *mode == 2, 46, 160, 67, "2 - MPI 2D");
        draw_button(button_serial_1d, *mode == 3, 120, 90, 170, "3 - SERIAL 1D");
        draw_button(button_serial_2d, *mode == 4, 160, 100, 60, "4 - SERIAL 2D");
        draw_button(button_toroidal, *mode == 5, 150, 70, 70, "5 - TOROIDAL 2D");

        if (is_1d_mode(*mode)) {
            draw_field(field_1, field == 0, "cells", *rows);
            draw_field(field_2, field == 1, "steps", *steps);
            draw_field(field_3, field == 2, "scale", *scale);
            draw_field(field_4, field == 3, "delay", *delay_ms);
        } else {
            draw_field(field_1, field == 0, "rows", *rows);
            draw_field(field_2, field == 1, "cols", *cols);
            draw_field(field_3, field == 2, "steps", *steps);
            draw_field(field_4, field == 3, "scale", *scale);
            draw_field(field_5, field == 4, "delay", *delay_ms);
            draw_field_text(field_6, field == 5, "pattern", pattern_name(*pattern_type));
        }

        if (valid) {
            draw_button(run_button, 0, 180, 120, 40, "RUN");
        } else {
            draw_button(run_button, 0, 160, 40, 40, "INVALID");
        }

        char info[256];

        if (valid) {
            snprintf(
                info,
                sizeof(info),
                "MPI processes: %d | 1=MPI1D | 2=MPI2D | 3=SERIAL1D | 4=SERIAL2D | 5=TOROIDAL2D | LEFT/RIGHT=select | UP/DOWN=modify | R=reset | ENTER=run",
                mpi_size
            );
        } else {
            snprintf(
                info,
                sizeof(info),
                "INVALID: cells/rows/cols trebuie sa fie divizibile cu numarul de procese MPI: %d",
                mpi_size
            );
        }

        SDL_Rect info_rect = {0, 940, 1920, 40};

        draw_centered_text(
            info,
            info_rect,
            font_small
        );

        SDL_RenderPresent(renderer);
        SDL_Delay(16);
    }

    close_window();

    return confirmed;
}

int sdl_init_viewer(int rows, int cols, int scale) {
    scale_global = scale;

    if (scale_global <= 0) {
        scale_global = 1;
    }

    int width = cols * scale_global;
    int height = rows * scale_global;

    if (width < 400) width = 400;
    if (height < 300) height = 300;

    if (width > 1600) width = 1600;
    if (height > 1000) height = 1000;

    return init_window("Game of Life MPI - running", width, height);
}

int sdl_render_grid(
    const unsigned char *grid,
    int rows,
    int cols,
    int delay_ms,
    const char *mode_name,
    int generation,
    int total_generations
) {
    static int paused = 0;

    SDL_Event event;

    char title[256];

    snprintf(
        title,
        sizeof(title),
        "Game of Life MPI - %s - Generation %d/%d",
        mode_name,
        generation,
        total_generations
    );

    SDL_SetWindowTitle(window, title);

    while (SDL_PollEvent(&event)) {

        if (event.type == SDL_QUIT) {
            return 0;
        }

        if (event.type == SDL_KEYDOWN) {

            if (event.key.keysym.sym == SDLK_ESCAPE) {
                return 0;
            }

            if (event.key.keysym.sym == SDLK_SPACE) {
                paused = !paused;
            }
        }
    }

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

    for (int r = 0; r < rows; r++) {
        for (int c = 0; c < cols; c++) {

            if (grid[r * cols + c]) {

                SDL_Rect rect = {
                    c * scale_global,
                    r * scale_global,
                    scale_global,
                    scale_global
                };

                SDL_RenderFillRect(renderer, &rect);
            }
        }
    }

    SDL_Color red = {255, 0, 0, 255};

    if (paused) {
        draw_text_color(
            "PAUSED - SPACE pentru resume",
            30,
            30,
            font_medium,
            red
        );
    }

    SDL_RenderPresent(renderer);

    while (paused) {

        while (SDL_PollEvent(&event)) {

            if (event.type == SDL_QUIT) {
                return 0;
            }

            if (event.type == SDL_KEYDOWN) {

                if (event.key.keysym.sym == SDLK_ESCAPE) {
                    return 0;
                }

                if (event.key.keysym.sym == SDLK_SPACE) {
                    paused = 0;
                }
            }
        }

        SDL_Delay(16);
    }

    SDL_Delay(delay_ms);

    return 1;
}

void sdl_wait_for_enter(const char *message, double elapsed) {
    int waiting = 1;

    SDL_Color red = {255, 0, 0, 255};

    char text[256];

    snprintf(
        text,
        sizeof(text),
        "%s | Timp: %.6f secunde | ENTER pentru meniu",
        message,
        elapsed
    );

    while (waiting) {
        SDL_Event event;

        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                waiting = 0;
            }

            if (event.type == SDL_KEYDOWN) {
                if (event.key.keysym.sym == SDLK_RETURN ||
                    event.key.keysym.sym == SDLK_KP_ENTER ||
                    event.key.keysym.sym == SDLK_ESCAPE) {
                    waiting = 0;
                }
            }
        }

        draw_text_color(
            text,
            30,
            30,
            font_medium,
            red
        );

        SDL_RenderPresent(renderer);
        SDL_Delay(16);
    }
}

void sdl_close_viewer(void) {
    close_window();
}
