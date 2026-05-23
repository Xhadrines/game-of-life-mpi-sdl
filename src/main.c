#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../include/core/parallel/gol_1d_parallel.h"
#include "../include/core/parallel/gol_2d_parallel.h"
#include "../include/core/parallel/gol_2d_parallel_blocks.h"
#include "../include/core/parallel/gol_2d_parallel_toroidal.h"
#include "../include/core/serial/gol_1d_serial.h"
#include "../include/core/serial/gol_2d_serial.h"
#include "../include/core/validation/gol_2d_validation.h"
#include "../include/ui/sdl_viewer.h"

int main(int argc, char **argv) {
    MPI_Init(&argc, &argv);

    int rank, size;

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (argc == 1 || strcmp(argv[1], "app") == 0) {
        int app_running = 1;

        while (app_running) {
            int mode = 0;

            int rows = 125 * size;
            int cols = 125 * size;
            int steps = 500 * size;
            int scale = 2;
            int delay_ms = 10;
            int pattern_type = 0;

            if (rank == 0) {
                if (!sdl_app_menu(size, &mode, &rows, &cols, &steps, &scale, &delay_ms, &pattern_type)) {
                    mode = 0;
                }
            }

            MPI_Bcast(&mode, 1, MPI_INT, 0, MPI_COMM_WORLD);
            MPI_Bcast(&rows, 1, MPI_INT, 0, MPI_COMM_WORLD);
            MPI_Bcast(&cols, 1, MPI_INT, 0, MPI_COMM_WORLD);
            MPI_Bcast(&steps, 1, MPI_INT, 0, MPI_COMM_WORLD);
            MPI_Bcast(&scale, 1, MPI_INT, 0, MPI_COMM_WORLD);
            MPI_Bcast(&delay_ms, 1, MPI_INT, 0, MPI_COMM_WORLD);
            MPI_Bcast(&pattern_type, 1, MPI_INT, 0, MPI_COMM_WORLD);

            if (mode == 0) {
                app_running = 0;
            }

            else if (mode == 1) {
                run_gol_1d_parallel_visual(
                    rows,
                    steps,
                    "rezultat_1d_parallel",
                    scale,
                    delay_ms
                );
            }

            else if (mode == 2) {
                run_gol_2d_parallel_visual(
                    rows,
                    cols,
                    steps,
                    "rezultat_2d_parallel",
                    scale,
                    delay_ms,
                    pattern_type
                );
            }

            else if (mode == 3) {
                if (rank == 0) {
                    run_gol_1d_serial_visual(
                        rows,
                        steps,
                        "rezultat_1d_serial",
                        scale,
                        delay_ms
                    );
                }
            }

            else if (mode == 4) {
                if (rank == 0) {
                    run_gol_2d_serial_visual(
                        rows,
                        cols,
                        steps,
                        "rezultat_2d_serial",
                        scale,
                        delay_ms,
                        pattern_type
                    );
                }
            }

            else if (mode == 5) {
                run_gol_2d_parallel_toroidal_visual(
                    rows,
                    cols,
                    steps,
                    "rezultat_2d_parallel_toroidal",
                    scale,
                    delay_ms,
                    pattern_type
                );
            }
        }

        MPI_Finalize();
        return 0;
    }

    if (strcmp(argv[1], "parallel1d") == 0 && argc == 5) {
        run_gol_1d_parallel(atoi(argv[2]), atoi(argv[3]), argv[4]);
    }

    else if (strcmp(argv[1], "parallel2d") == 0 && argc == 7) {
        run_gol_2d_parallel(atoi(argv[2]), atoi(argv[3]), atoi(argv[4]), argv[5], atoi(argv[6]));
    }

    else if (strcmp(argv[1], "parallel2d_blocks") == 0 && argc == 7) {
        run_gol_2d_parallel_blocks(
            atoi(argv[2]),
            atoi(argv[3]),
            atoi(argv[4]),
            argv[5],
            atoi(argv[6])
        );
    }

    else if (strcmp(argv[1], "parallel2d_toroidal") == 0 && argc == 7) {
        run_gol_2d_parallel_toroidal(
            atoi(argv[2]),
            atoi(argv[3]),
            atoi(argv[4]),
            argv[5],
            atoi(argv[6])
        );
    }

    else if (strcmp(argv[1], "serial1d") == 0 && argc == 5) {
        run_gol_1d_serial(
            atoi(argv[2]),
            atoi(argv[3]),
            argv[4]
        );
    }

    else if (strcmp(argv[1], "serial2d") == 0 && argc == 7) {
        run_gol_2d_serial(
            atoi(argv[2]),
            atoi(argv[3]),
            atoi(argv[4]),
            argv[5],
            atoi(argv[6])
        );
    }

    else if (strcmp(argv[1], "validate2d") == 0 && argc == 6) {
        run_gol_2d_validation(
            atoi(argv[2]),
            atoi(argv[3]),
            atoi(argv[4]),
            atoi(argv[5])
        );
    }

    MPI_Finalize();
    return 0;
}
