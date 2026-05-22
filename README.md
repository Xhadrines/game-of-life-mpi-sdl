# Conway Game of Life - MPI + SDL2

Implementare paralelă a jocului Conway's Game of Life folosind MPI și SDL2.

## Funcționalități

- Simulare Conway Game of Life
- Mod 1D și 2D
- Procesare paralelă folosind MPI
- Interfață grafică SDL2
- Configurare runtime prin meniu interactiv
- Sistem Pause / Resume
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

### Rulare cu 4 procese MPI:

```bash
make run
```

### Rulare cu 8 procese MPI:

```bash
make run8
```

### Sau manual:

```bash
mpirun -np 4 ./build/gol_mpi app
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
│   │   ├── gol_1d.h
│   │   └── gol_2d.h
│   ├── ui/
│   │   └── sdl_viewer.h
│   └── utils/
│       └── utils.h
│
├── output/
│
├── src/
│   ├── core/
│   │   ├── gol_1d.c
│   │   └── gol_2d.c
│   ├── ui/
│   │   └── sdl_viewer.c
│   ├── utils/
│   │   └── utils.c
│   └── main.c
│
├── .gitignore
├── LICENSE
├── Makefile
└── README.md
```

## Controale

### Meniu principal

| Tastă        | Acțiune                   |
| ------------ | ------------------------- |
| 1            | Selectare mod 1D          |
| 2            | Selectare mod 2D          |
| LEFT / RIGHT | Schimbare câmp selectat   |
| UP / DOWN    | Creștere / scădere valori |
| ENTER        | Pornire simulare          |
| R            | Reset valori implicite    |
| ESC          | Ieșire                    |

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
