#ifndef UTILS_H
#define UTILS_H

#define TXT_EXPORT_MAX_ROWS 500
#define TXT_EXPORT_MAX_COLS 500

/*
 * Functie: alloc_grid
 * Ce face: aloca memorie pentru un grid (rows x cols) si inisializeaza cu 0.
 * Foloseste: cand ai nevoie de un buffer pentru starea celulelor.
 * Parametri:
 *   - rows: numar randuri (trebuie > 0)
 *   - cols: numar coloane (trebuie > 0)
 * Returneaza: pointer la zona alocata (unsigned char*) sau NULL la eroare.
 * Atensionare: elibereaza memoria cu free() dupa folosire.
 */
unsigned char *alloc_grid(
    int rows,
    int cols
);

/*
 * Functie: random_fill
 * Ce face: umple grid-ul cu 0/1 pe baza probabilitasii alive_prob.
 * Parametri:
 *   - grid: buffer alocat (rows*cols)
 *   - rows, cols: dimensiuni
 *   - alive_prob: valoare intre 0.0 si 1.0 (ex: 0.3 pentru 30% vie)
 *   - seed: seed pentru srand() pentru reproducibilitate
 * Atensionare: nu verifica grid != NULL, deci verifica inainte de apel.
 */
void random_fill(
    unsigned char *grid,
    int rows,
    int cols,
    double alive_prob,
    unsigned int seed
);

/*
 * Functie: write_pgm
 * Ce face: scrie grid-ul in format PGM (P2). Valorile sunt 0 sau 255.
 * Parametri:
 *   - path: cale catre fisierul de iesire
 *   - grid: pointer la date (0/1)
 *   - rows, cols: dimensiuni
 * Returneaza: 1 la succes, 0 la eroare.
 */
int write_pgm(
    const char *path,
    const unsigned char *grid,
    int rows,
    int cols
);

/*
 * Functie: write_ppm
 * Ce face: scrie grid-ul in format PPM (P3). Celulele vii devin albe (255 255 255), cele moarte negre.
 */
int write_ppm(
    const char *path,
    const unsigned char *grid,
    int rows,
    int cols
);

/*
 * Functie: write_txt
 * Ce face: scrie grid-ul intr-un fisier text folosind '#' pentru vie si '.' pentru moarta.
 * Observasie: folosit doar cand grid-ul este relativ mic (vezi TXT_EXPORT_MAX_*).
 */
int write_txt(
    const char *path,
    const unsigned char *grid,
    int rows,
    int cols
);

/*
 * Functie: write_output_files
 * Ce face: wrapper care scrie PGM, PPM si (opsional) TXT pentru un grid.
 * Parametri: base_name va fi folosit pentru a construi numele fisierelor in output/.
 */
int write_output_files(
    const char *base_name,
    const unsigned char *grid,
    int rows,
    int cols
);

/*
 * Functie: append_benchmark_csv
 * Ce face: adauga o linie in output/benchmarks/benchmark_results.csv cu informasii despre rulare.
 * Parametri: run_name, mode (ex: "parallel2d"), processes, rows, cols, steps, timpi
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
);

#endif
