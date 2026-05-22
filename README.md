# Conway Game of Life - MPI + SDL2

Implementare paralelă a jocului Conway's Game of Life folosind MPI și SDL2.

## Funcționalități

- Simulare Conway Game of Life
- Mod 1D și 2D
- Implementare serială pentru 1D și 2D
- Implementare paralelă pentru 1D și 2D folosind MPI
- Rulare din terminal pentru variantele seriale și paralele
- Interfață grafică SDL2
- Configurare runtime prin meniu interactiv
- Selectare mod din interfață:
  - MPI 1D
  - MPI 2D
  - Serial 1D
  - Serial 2D
- Sistem Pause / Resume
- Oprire simulare cu salvarea ultimei stări
- Titlu dinamic al ferestrei cu modul curent și generația simulată
- Export imagine finală (.pgm)
- Validare automată pentru împărțirea corectă a gridului între procese MPI
- Control complet din tastatură
- Creare automată pentru directoarele build/ și output/

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
cd conway-game-of-life-mpi
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
mpirun -np 4 ./build/gol_mpi parallel1d 1000 500 output/parallel1d.pgm
```

#### 2D Parallel

```bash
make parallel2d
```

#### Sau manual:

```bash
mpirun -np 4 ./build/gol_mpi parallel2d 500 500 1000 output/parallel2d.pgm
```

---

### Rulare Serial Console

#### 1D Serial

```bash
make serial1d
```

#### Sau manual:

```bash
mpirun -np 4 ./build/gol_mpi serial1d 1000 500 output/serial1d.pgm
```

#### 2D Serial

```bash
make serial2d
```

#### Sau manual:

```bash
mpirun -np 4 ./build/gol_mpi serial2d 500 500 1000 output/serial2d.pgm
```

## Structura proiectului

```text
game_of_life/
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
│   │   │   └── gol_2d_parallel.h
│   │   │
│   │   └── serial/
│   │       ├── gol_1d_serial.h
│   │       └── gol_2d_serial.h
│   │
│   ├── ui/
│   │   └── sdl_viewer.h
│   │
│   └── utils/
│       └── utils.h
│
├── output/
│
├── src/
│   ├── core/
│   │   ├── parallel/
│   │   │   ├── gol_1d_parallel.c
│   │   │   └── gol_2d_parallel.c
│   │   │
│   │   └── serial/
│   │       ├── gol_1d_serial.c
│   │       └── gol_2d_serial.c
│   │
│   ├── ui/
│   │   └── sdl_viewer.c
│   │
│   ├── utils/
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

| Tastă        | Acțiune                    |
| ------------ | -------------------------- |
| 1            | Selectare mod MPI 1D       |
| 2            | Selectare mod MPI 2D       |
| 3            | Selectare mod Serial 1D    |
| 4            | Selectare mod Serial 2D    |
| LEFT / RIGHT | Schimbare câmp selectat    |
| UP / DOWN    | Creștere / scădere valori  |
| ENTER        | Pornire simulare           |
| SPACE        | Pause / Resume în simulare |
| R            | Reset valori implicite     |
| ESC          | Ieșire / oprire simulare   |

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
output/rezultat_2d_sdl.pgm
```
