CC=mpicc

CFLAGS=-Wall -Wextra -O2 -Iinclude $(shell sdl2-config --cflags)

LDFLAGS=$(shell sdl2-config --libs) -lSDL2_ttf

TARGET=build/gol_mpi

SRC=src/main.c \
    src/core/parallel/gol_1d_parallel.c \
    src/core/parallel/gol_2d_parallel.c \
	src/core/serial/gol_1d_serial.c \
    src/core/serial/gol_2d_serial.c \
    src/utils/utils.c \
    src/ui/sdl_viewer.c

all: $(TARGET)

$(TARGET): $(SRC)
	mkdir -p build
	mkdir -p output

	$(CC) $(CFLAGS) $(SRC) -o $(TARGET) $(LDFLAGS)

# =========================
# SDL APP
# =========================

run: $(TARGET)
	mpirun -np 4 ./$(TARGET) app

run8: $(TARGET)
	mpirun -np 8 ./$(TARGET) app

# =========================
# PARALLEL CONSOLE
# =========================

parallel1d: $(TARGET)
	mpirun -np 4 ./$(TARGET) parallel1d 1000 500 output/parallel1d.pgm

parallel2d: $(TARGET)
	mpirun -np 4 ./$(TARGET) parallel2d 500 500 1000 output/parallel2d.pgm

# =========================
# SERIAL CONSOLE
# =========================

serial1d: $(TARGET)
	./$(TARGET) serial1d 1000 500 output/serial1d.pgm

serial2d: $(TARGET)
	./$(TARGET) serial2d 500 500 1000 output/serial2d.pgm

# =========================
# CLEAN
# =========================

clean:
	rm -f build/* output/*.pgm
