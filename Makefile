CC=mpicc

CFLAGS=-Wall -Wextra -O2 -Iinclude $(shell sdl2-config --cflags)
LDFLAGS=$(shell sdl2-config --libs) -lSDL2_ttf

TARGET=build/gol_mpi

SRC=src/main.c \
    src/core/gol_1d.c \
    src/core/gol_2d.c \
    src/utils/utils.c \
    src/ui/sdl_viewer.c

all: $(TARGET)

$(TARGET): $(SRC)
	mkdir -p build
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET) $(LDFLAGS)

run: $(TARGET)
	mpirun -np 4 ./$(TARGET) app

run8: $(TARGET)
	mpirun -np 8 ./$(TARGET) app

clean:
	rm -f build/* output/*.pgm
