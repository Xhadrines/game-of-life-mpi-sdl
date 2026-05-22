#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../include/core/gol_1d.h"
#include "../include/core/gol_2d.h"
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

            if (rank == 0) {
                if (!sdl_app_menu(size, &mode, &rows, &cols, &steps, &scale, &delay_ms)) {
                    mode = 0;
                }
            }

            MPI_Bcast(&mode, 1, MPI_INT, 0, MPI_COMM_WORLD);
            MPI_Bcast(&rows, 1, MPI_INT, 0, MPI_COMM_WORLD);
            MPI_Bcast(&cols, 1, MPI_INT, 0, MPI_COMM_WORLD);
            MPI_Bcast(&steps, 1, MPI_INT, 0, MPI_COMM_WORLD);
            MPI_Bcast(&scale, 1, MPI_INT, 0, MPI_COMM_WORLD);
            MPI_Bcast(&delay_ms, 1, MPI_INT, 0, MPI_COMM_WORLD);

            if (mode == 0) {
                app_running = 0;
            } else if (mode == 1) {
                run_gol_1d_visual(rows, steps, "output/rezultat_1d_sdl.pgm", scale, delay_ms);
            } else if (mode == 2) {
                run_gol_2d_visual(rows, cols, steps, "output/rezultat_2d_sdl.pgm", scale, delay_ms);
            }
        }

        MPI_Finalize();
        return 0;
    }

    if (strcmp(argv[1], "1d") == 0 && argc == 5) {
        run_gol_1d(atoi(argv[2]), atoi(argv[3]), argv[4]);
    }

    else if (strcmp(argv[1], "2d") == 0 && argc == 6) {
        run_gol_2d(atoi(argv[2]), atoi(argv[3]), atoi(argv[4]), argv[5]);
    }

    MPI_Finalize();
    return 0;
}
