# Conway Game of Life - MPI + SDL2

Implementare paralelă a jocului Conway's Game of Life folosind MPI și SDL2.

## Funcționalități

- Simulare Conway Game of Life
- Mod 1D și 2D
- Implementare serială pentru 1D și 2D
- Implementare paralelă pentru 1D și 2D folosind MPI
- Implementare paralelă 2D cu grilă toroidală / periodică
- Rulare din terminal pentru variantele seriale și paralele
- Rulare benchmark pentru grile mari `10000 x 10000`
- Interfață grafică SDL2
- Configurare runtime prin meniu interactiv
- Selectare mod din interfață:
  - MPI 1D
  - MPI 2D
  - MPI 2D Toroidal
  - Serial 1D
  - Serial 2D
- Pattern-uri clasice Conway pentru simulările 2D:
  - random
  - glider
  - blinker
- Selectare pattern din interfața SDL2 și terminal pentru modurile 2D
- Modificare rapidă valori cu `SHIFT + UP / DOWN`
- Sistem Pause / Resume
- Oprire simulare cu salvarea ultimei stări
- Titlu dinamic al ferestrei cu modul curent și generația simulată
- Afișare timp de execuție la finalul simulării
- Export imagine finală `.pgm` și `.ppm`
- Validare automată pentru împărțirea corectă a gridului între procese MPI
- Control complet din tastatură
- Creare automată pentru directoarele `build/` și `output/`

## Tehnologii folosite

- GCC (v16.1.1 sau o versiune mai mare)
- OpenMPI (v5.0.10 sau o versiune mai mare)
- SDL2
- SDL2_ttf

## Instalare

1. Clonează repository-ul

```bash
git clone https://github.com/Xhadrines/game-of-life-mpi-sdl.git
```

2. Intră în directorul proiectului

```bash
cd game-of-life-mpi-sdl
```

3. Instalează dependențele (Arch Linux)

```bash
sudo pacman -S gcc openmpi sdl2 sdl2_ttf ttf-dejavu
```

## Compilare

```bash
make
```

## Rulare

> Pattern-uri disponibile pentru modurile 2D:
>
> - `0` = random
> - `1` = glider
> - `2` = blinker

### Interfață SDL2

#### Rulare cu 4 procese MPI:

```bash
make run
```

#### Rulare cu 8 procese MPI:

```bash
make run8
```

#### Sau manual:

```bash
mpirun -np 4 ./build/gol_mpi app
```

---

### Rulare Parallel Console

#### 1D Parallel

```bash
make parallel1d
```

#### Sau manual:

```bash
mpirun -np 4 ./build/gol_mpi parallel1d 1000 500 parallel1d
```

#### 2D Parallel

```bash
make parallel2d
```

#### Sau manual:

```bash
mpirun -np 4 ./build/gol_mpi parallel2d 500 500 1000 parallel2d 0
```

#### 2D Parallel Toroidal

```bash
make parallel2d_toroidal
```

#### Sau manual:

```bash
mpirun -np 4 ./build/gol_mpi parallel2d_toroidal 500 500 1000 parallel2d_toroidal 0
```

---

### Rulare Serial Console

#### 1D Serial

```bash
make serial1d
```

#### Sau manual:

```bash
./build/gol_mpi serial1d 1000 500 serial1d
```

#### 2D Serial

```bash
make serial2d
```

#### Sau manual:

```bash
./build/gol_mpi serial2d 500 500 1000 serial2d 0
```

---

### HPC Benchmarks (console only)

#### Parallel 2D Benchmark (10000x10000)

```bash
make benchmark2d_parallel
```

#### Sau manual

```bash
mpirun -np 8 ./build/gol_mpi parallel2d 10000 10000 100 benchmark_2d_parallel 0
```

#### Parallel 2D Toroidal Benchmark (10000x10000)

```bash
make benchmark2d_parallel_toroidal
```

#### Sau manual

```bash
mpirun -np 8 ./build/gol_mpi parallel2d_toroidal 10000 10000 100 benchmark_2d_toroidal 0
```

#### Serial 2D Benchmark (10000x10000)

```bash
make benchmark2d_serial
```

#### Sau manual

```bash
./build/gol_mpi serial2d 10000 10000 100 benchmark_2d_serial 0
```

## Structura proiectului

```text
game-of-life-mpi-sdl/
│
├── assets/
│   └── fonts/
│       └── DejaVuSans.ttf
│
├── build/
│
├── include/
│   ├── core/
│   │   ├── parallel/
│   │   │   ├── gol_1d_parallel.h
│   │   │   ├── gol_2d_parallel.h
│   │   │   └── gol_2d_parallel_toroidal.h
│   │   │
│   │   └── serial/
│   │       ├── gol_1d_serial.h
│   │       └── gol_2d_serial.h
│   │
│   ├── ui/
│   │   └── sdl_viewer.h
│   │
│   └── utils/
│       ├── patterns.h
│       └── utils.h
│
├── output/
│   ├── pgm/
│   └── ppm/
│
├── src/
│   ├── core/
│   │   ├── parallel/
│   │   │   ├── gol_1d_parallel.c
│   │   │   ├── gol_2d_parallel.c
│   │   │   └── gol_2d_parallel_toroidal.c
│   │   │
│   │   └── serial/
│   │       ├── gol_1d_serial.c
│   │       └── gol_2d_serial.c
│   │
│   ├── ui/
│   │   └── sdl_viewer.c
│   │
│   ├── utils/
│   │   ├── patterns.c
│   │   └── utils.c
│   │
│   └── main.c
│
├── .gitignore
├── LICENSE
├── Makefile
└── README.md
```

## Controale

### Meniu principal

| Tastă             | Acțiune                                        |
| ----------------- | ---------------------------------------------- |
| 1                 | Selectare mod MPI 1D                           |
| 2                 | Selectare mod MPI 2D                           |
| 3                 | Selectare mod Serial 1D                        |
| 4                 | Selectare mod Serial 2D                        |
| 5                 | Selectare mod MPI 2D Toroidal                  |
| LEFT / RIGHT      | Schimbare câmp selectat                        |
| UP / DOWN         | Creștere / scădere valori câmp selectat        |
| SHIFT + UP / DOWN | Creștere / scădere rapidă valori câmp selectat |
| ENTER             | Pornire simulare                               |
| SPACE             | Pause / Resume în simulare                     |
| R                 | Reset valori implicite                         |
| ESC               | Ieșire / oprire simulare                       |

### În timpul simulării

| Tastă | Acțiune                                                |
| ----- | ------------------------------------------------------ |
| SPACE | Pause / Resume                                         |
| ESC   | Oprire simulare și salvare imagine                     |
| ENTER | Revenire la meniul principal după terminarea simulării |

## Output

Imaginile generate sunt salvate automat în:

```
output/
```

Exemplu:

```
output/pgm/rezultat_2d_parallel.pgm
output/ppm/rezultat_2d_parallel.ppm
output/pgm/rezultat_2d_parallel_toroidal.pgm
output/ppm/rezultat_2d_parallel_toroidal.ppm
output/pgm/rezultat_2d_serial.pgm
output/ppm/rezultat_2d_serial.ppm
```
