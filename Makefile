CC=mpicc

CFLAGS=-Wall -Wextra -O2 -Iinclude $(shell sdl2-config --cflags)

LDFLAGS=$(shell sdl2-config --libs) -lSDL2_ttf

TARGET=build/gol_mpi

SRC=src/main.c \
    src/core/parallel/gol_1d_parallel.c \
    src/core/parallel/gol_2d_parallel.c \
	src/core/parallel/gol_2d_parallel_toroidal.c \
	src/core/serial/gol_1d_serial.c \
    src/core/serial/gol_2d_serial.c \
    src/utils/utils.c \
	src/utils/patterns.c \
    src/ui/sdl_viewer.c

all: $(TARGET)

$(TARGET): $(SRC)
	mkdir -p build
	mkdir -p output
	mkdir -p output/pgm
	mkdir -p output/ppm
	mkdir -p output/txt

	$(CC) $(CFLAGS) $(SRC) -o $(TARGET) $(LDFLAGS)

# =========================
# SDL APP
# =========================

run: $(TARGET)
	mpirun -np 4 ./$(TARGET) app

run8: $(TARGET)
	mpirun -np 8 ./$(TARGET) app

run12: $(TARGET)
	mpirun --oversubscribe -np 12 ./$(TARGET) app	# Add --oversubscribe flag to allow more processes than available CPU cores

run16: $(TARGET)
	mpirun --oversubscribe -np 16 ./$(TARGET) app	# Add --oversubscribe flag to allow more processes than available CPU cores

# =========================
# PARALLEL CONSOLE
# =========================

parallel1d: $(TARGET)
	mpirun -np 4 ./$(TARGET) parallel1d 1000 500 parallel1d

parallel2d: $(TARGET)
	mpirun -np 4 ./$(TARGET) parallel2d 500 500 1000 parallel2d 0

parallel2d_toroidal: $(TARGET)
	mpirun -np 4 ./$(TARGET) parallel2d_toroidal 500 500 1000 parallel2d_toroidal 0

# =========================
# SERIAL CONSOLE
# =========================

serial1d: $(TARGET)
	./$(TARGET) serial1d 1000 500 serial1d

serial2d: $(TARGET)
	./$(TARGET) serial2d 500 500 1000 serial2d 0

# =========================
# HPC BENCHMARKS
# =========================

benchmark2d_parallel: $(TARGET)
	@echo "Please wait... running MPI 2D benchmark 10000x10000"
	mpirun -np 8 ./$(TARGET) parallel2d 10000 10000 100 benchmark_2d_parallel 0

benchmark2d_parallel_toroidal: $(TARGET)
	@echo "Please wait... running TOROIDAL MPI 2D benchmark 10000x10000"
	mpirun -np 8 ./$(TARGET) parallel2d_toroidal 10000 10000 100 benchmark_2d_toroidal 0

benchmark2d_serial: $(TARGET)
	@echo "Please wait... running SERIAL 2D benchmark 10000x10000"
	./$(TARGET) serial2d 10000 10000 100 benchmark_2d_serial 0

# =========================
# CLEAN
# =========================

clean:
	rm -f build/* output/pgm/*.pgm output/ppm/*.ppm output/txt/*.txt
