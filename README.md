# Conway Game of Life - MPI + SDL2

Implementare paralelă a jocului Conway's Game of Life folosind MPI și SDL2.

## Funcționalități

- Simulare Conway Game of Life
- Mod 1D și 2D
- Implementare serială pentru 1D și 2D
- Implementare paralelă pentru 1D și 2D folosind MPI
- Descompunere domeniu prin stripes / 1D decomposition
- Halo exchange cu ghost cells între procese MPI
- Comunicații non-blocking folosind `MPI_Isend`, `MPI_Irecv` și `MPI_Waitall`
- Evitare deadlock prin recepții non-blocking, trimiteri non-blocking și sincronizare cu `MPI_Waitall`
- Implementare paralelă 2D cu grilă toroidală / periodică
- Rulare din terminal pentru variantele seriale și paralele
- Rulare benchmark pentru grile mari `10000 x 10000`
- Rulare locală cu 4, 8, 12 și 16 procese MPI
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
- Export automat pentru fișiere `.pgm`, `.ppm` și `.txt` (`.txt` doar pentru grile mici)
- Validare automată pentru împărțirea corectă a gridului între procese MPI
- Control complet din tastatură
- Măsurare performanță cu `MPI_Wtime`: timp total, timp comunicație și timp calcul
- Export rezultate benchmark în format `.csv`
- Generare grafice pentru speedup, eficiență, weak scaling și timp calcul vs comunicație
- Validare bit-cu-bit între implementarea serială și MPI
- Reproducibilitate folosind seed determinist
- Creare automată pentru directoarele `build/` și `output/*`

## Tehnologii folosite

- GCC (v16.1.1 sau o versiune mai mare)
- OpenMPI (v5.0.10 sau o versiune mai mare)
- SDL2
- SDL2_ttf
- Python 3
- pandas
- matplotlib

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

4. Creează mediul virtual Python pentru benchmark analysis

```bash
make venv
```

## Compilare

```bash
make
```

## Rulare

### Interfață SDL2

#### Rulare cu 4 procese MPI:

```bash
make run
```

#### Rulare cu 8 procese MPI:

```bash
make run8
```

#### Rulare cu 12 procese MPI:

```bash
make run12
```

#### Rulare cu 16 procese MPI:

```bash
make run16
```

---

### Rulare Parallel Console

#### 1D Parallel

```bash
make parallel1d
```

#### 2D Parallel

```bash
make parallel2d
```

#### 2D Parallel Toroidal

```bash
make parallel2d_toroidal
```

---

### Rulare Serial Console

#### 1D Serial

```bash
make serial1d
```

#### 2D Serial

```bash
make serial2d
```

---

### HPC Benchmarks (console only)

#### Parallel 2D Benchmark (10000x10000)

```bash
make benchmark2d_parallel
```

#### Parallel 2D Toroidal Benchmark (10000x10000)

```bash
make benchmark2d_parallel_toroidal
```

#### Serial 2D Benchmark (10000x10000)

```bash
make benchmark2d_serial
```

#### Strong Scaling Benchmark (10000x10000)

```bash
make strong_scaling
```

#### Weak Scaling Benchmark (10000x10000)

```bash
make weak_scaling
```

## Analiză benchmark și generare grafice

```bash
make analyze_benchmarks
```

## Validare serial vs MPI

```bash
make validate2d
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
│   │   ├── serial/
│   │   │   ├── gol_1d_serial.h
│   │   │   └── gol_2d_serial.h
│   │   │
│   │   └── validation/
│   │       └── gol_2d_validation.h
│   │
│   ├── ui/
│   │   └── sdl_viewer.h
│   │
│   └── utils/
│       ├── patterns.h
│       └── utils.h
│
├── output/
│   ├── benchmarks/
│   ├── graphs/
│   ├── pgm/
│   ├── ppm/
│   └── txt/
│
├── scripts/
│   └── analyze_benchmarks.py
│
├── src/
│   ├── core/
│   │   ├── parallel/
│   │   │   ├── gol_1d_parallel.c
│   │   │   ├── gol_2d_parallel.c
│   │   │   └── gol_2d_parallel_toroidal.c
│   │   │
│   │   ├── serial/
│   │   │   ├── gol_1d_serial.c
│   │   │   └── gol_2d_serial.c
│   │   │
│   │   └── validation/
│   │       └── gol_2d_validation.h
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

Fișierele generate sunt salvate automat în:

```
output/
```

Exemplu:

```
output/pgm/rezultat_2d_parallel.pgm
output/ppm/rezultat_2d_parallel.ppm
output/txt/rezultat_2d_parallel.txt
output/benchmarks/benchmark_results.csv
output/benchmarks/strong_scaling_analysis.csv
output/benchmarks/weak_scaling_analysis.csv
output/graphs/speedup.png
output/graphs/efficiency.png
output/graphs/communication_vs_computation.png
output/graphs/weak_scaling.png
```
