# c-simulations
A collection of various simulations, all built in C.

## Current Features

### Eulerian Fluid Simulation

- 2D Eulerian fluid simulation
- Interaction with mouse input
- Rendering with SDL2

## Setup Instructions

1. Install MSYS2 from [https://www.msys2.org/](https://www.msys2.org/):

2. In MSYS2 MinGW 64-bit terminal, run the following commands to install the necessary tools:
```bash
pacman -Syu
```
```bash
pacman -S mingw-w64-x86_64-make mingw-w64-x86_64-pkg-config mingw-w64-x86_64-gcc mingw-w64-x86_64-SDL2
```

3. Clone the Repository:
```bash
git clone [c-simulations] /c/c-simulations
```
```bash
cd /c/c-simulations
```

## Buiding the Project

1. Navigate to your projects directory in **MSYS2 MinGW 64-bit terminal**:

```bash
cd /c/c-simulations
```

2. Activate the makefile:

```bash
make
```

3. Navigate to the 'build' directory:

```bash
cd build
```

4. Run the simulation:

```bash
./sdl_gui_app.exe
```

5. Clean the project:

```bash
make clean
```
