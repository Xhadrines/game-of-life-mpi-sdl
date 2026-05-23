#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../include/utils/utils.h"

/*
 * Functie: alloc_grid
 * Ce face: aloca un buffer pentru grid (rows x cols) si inisializeaza cu 0.
 * Parametri: rows, cols - dimensiunile gridului.
 * Returneaza: pointer la memorie (unsigned char*) sau NULL la eroare.
 * Atensionari: Verifica intotdeauna returnul in apeluri; foloseste `free()`.
 */
unsigned char *alloc_grid(int rows, int cols) {
    if (rows <= 0 || cols <= 0) return NULL;

    return calloc((size_t)rows * (size_t)cols, sizeof(unsigned char));
}

/*
 * Functie: random_fill
 * Ce face: completeaza grid-ul cu valori 0/1 pe baza probabilitasii `alive_prob`.
 * Parametri:
 *   - grid: buffer prealocat de dimensiune rows*cols
 *   - rows, cols: dimensiunile gridului
 *   - alive_prob: probabilitatea ca o celula sa fie vie (0..1)
 *   - seed: seed pentru rand() (reproducibilitate)
 * Returneaza: nimic (efecte asupra `grid`).
 * Atensionari: Daca `grid` este NULL, nu face nimic.
 */
void random_fill(unsigned char *grid, int rows, int cols, double alive_prob, unsigned int seed) {
    if (!grid) return;

    srand(seed);

    for (int i = 0; i < rows * cols; i++) {
        double r = (double)rand() / (double)RAND_MAX;
        grid[i] = r < alive_prob ? 1 : 0;
    }
}

/*
 * Functie: write_pgm
 * Ce face: scrie imaginea in format PGM (P2) pe disk.
 * Parametri: path (cale fisier), grid (0/1), rows, cols
 * Returneaza: 1 la succes, 0 la eroare (si afiseaza perror()).
 * Atensionari: Formatele PGM/P3 folosite de proiect asteapta valori 0/255.
 */
int write_pgm(const char *path, const unsigned char *grid, int rows, int cols) {
    FILE *f = fopen(path, "w");

    if (!f) {
        perror("fopen");
        return 0;
    }

    fprintf(f, "P2\n%d %d\n255\n", cols, rows);

    for (int r = 0; r < rows; r++) {
        for (int c = 0; c < cols; c++) {
            fprintf(f, "%d ", grid[r * cols + c] ? 255 : 0);
        }

        fprintf(f, "\n");
    }

    fclose(f);
    return 1;
}

/*
 * Functie: write_ppm
 * Ce face: scrie imaginea in format PPM (P3) pe disk; celulele vii devin albe.
 * Parametri: path, grid, rows, cols
 * Returneaza: 1 la succes, 0 la eroare.
 */
int write_ppm(const char *path, const unsigned char *grid, int rows, int cols) {
    FILE *f = fopen(path, "w");

    if (!f) {
        perror("fopen");
        return 0;
    }

    fprintf(f, "P3\n%d %d\n255\n", cols, rows);

    for (int r = 0; r < rows; r++) {
        for (int c = 0; c < cols; c++) {
            if (grid[r * cols + c]) {
                fprintf(f, "255 255 255 ");
            } else {
                fprintf(f, "0 0 0 ");
            }
        }

        fprintf(f, "\n");
    }

    fclose(f);
    return 1;
}

/*
 * Functie: write_txt
 * Ce face: scrie grid-ul intr-un fisier text folosind '#' pentru vie si '.' pentru moarta.
 * Parametri: path, grid, rows, cols
 * Returneaza: 1 la succes, 0 la eroare.
 * Atensionari: Apelata doar cand grid-ul e mic (vezi `TXT_EXPORT_MAX_*`).
 */
int write_txt(
    const char *path,
    const unsigned char *grid,
    int rows,
    int cols
) {
    FILE *f = fopen(path, "w");

    if (!f) {
        perror("fopen");
        return 0;
    }

    for (int r = 0; r < rows; r++) {
        for (int c = 0; c < cols; c++) {
            fprintf(
                f,
                "%c ",
                grid[r * cols + c] ? '#' : '.'
            );
        }

        fprintf(f, "\n");
    }

    fclose(f);
    return 1;
}

/*
 * Functie: write_output_files
 * Ce face: wrapper ce scrie PGM, PPM si (opsional) TXT pentru un grid.
 * Parametri: base_name (prefix pentru fisiere), grid, rows, cols
 * Returneaza: 1 daca toate scrierile necesare reusesc, altfel 0.
 */
int write_output_files(
    const char *base_name,
    const unsigned char *grid,
    int rows,
    int cols
) {
    char pgm_full[512];
    char ppm_full[512];
    char txt_full[512];

    snprintf(
        pgm_full,
        sizeof(pgm_full),
        "output/pgm/%s.pgm",
        base_name
    );

    snprintf(
        ppm_full,
        sizeof(ppm_full),
        "output/ppm/%s.ppm",
        base_name
    );

    snprintf(
        txt_full,
        sizeof(txt_full),
        "output/txt/%s.txt",
        base_name
    );

    int ok_pgm = write_pgm(pgm_full, grid, rows, cols);
    int ok_ppm = write_ppm(ppm_full, grid, rows, cols);

    int ok_txt = 1;

    if (
        rows <= TXT_EXPORT_MAX_ROWS &&
        cols <= TXT_EXPORT_MAX_COLS
    ) {
        ok_txt = write_txt(
            txt_full,
            grid,
            rows,
            cols
        );
    }

    return ok_pgm && ok_ppm && ok_txt;
}

/*
 * Functie: append_benchmark_csv
 * Ce face: adauga un rand in `output/benchmarks/benchmark_results.csv` cu timpii.
 * Parametri: run_name, mode, processes, rows, cols, steps, total_time, communication_time, computation_time
 * Returneaza: 1 la succes, 0 la eroare.
 * Atensionari: Creeaza header-ul daca fisierul nu exista.
 */
int append_benchmark_csv(
    const char *run_name,
    const char *mode,
    int processes,
    int rows,
    int cols,
    int steps,
    double total_time,
    double communication_time,
    double computation_time
) {
    FILE *check = fopen("output/benchmarks/benchmark_results.csv", "r");

    int file_exists = check != NULL;

    if (check) {
        fclose(check);
    }

    FILE *f = fopen("output/benchmarks/benchmark_results.csv", "a");

    if (!f) {
        perror("fopen");
        return 0;
    }

    if (!file_exists) {
        fprintf(f, "run_name,mode,processes,rows,cols,steps,total_time,communication_time,computation_time,communication_percent\n");
        
    }

    double communication_percent = 0.0;

    if (total_time > 0.0) {
        communication_percent =
            (communication_time / total_time) * 100.0;
    }

    fprintf(
        f,
        "%s,%s,%d,%d,%d,%d,%.6f,%.6f,%.6f,%.2f\n",
        run_name,
        mode,
        processes,
        rows,
        cols,
        steps,
        total_time,
        communication_time,
        computation_time,
        communication_percent
    );

    fclose(f);

    return 1;
}
