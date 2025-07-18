# c-simulations
A collection of various simulations, all built in C.

## Current Features

### Eulerian Fluid Simulation

![Demo of Eulerian Fluid Simulation](assets/gifs/eulerian_smoke.gif)

- 2D Eulerian fluid simulation
- Interaction with mouse input
- Rendering with SDL2

### Ray Tracing Simulation

![Demo of Ray Tracer Simulation](assets/images/ray_traced_spheres.png)

- Ray traced 3D scene
- Sphere scene objects
- Ambient, diffuse, and specular lighting
- Soft shadows
- Reflections
- Mouse interaction to control light source[^1]
- Rendering with SDL2

[^1]: Note: This is a single-threaded application and has limited GPU acceleration.  As a result, blazingly fast real time rendering is not expected.

### Terrain Generation Simulation

![Demo of 2D Terrain Generator](assets/images/2D_terrain_generation.png)

- Perlin noise-based 2D terrain generation
- Height-based coloring
- Rendering with SDL2

## Setup Instructions

1. Install MSYS2 from [https://www.msys2.org/](https://www.msys2.org/):

2. In MSYS2 MinGW 64-bit terminal, run the following commands to install Make, pkg-config, and SDL2.
```bash
pacman -Syu
```
```bash
pacman -S mingw-w64-x86_64-make mingw-w64-x86_64-pkg-config mingw-w64-x86_64-gcc mingw-w64-x86_64-SDL2
```

3. Clone the Repository:
```bash
git clone https://github.com/SpencerF718/c-simulations /c/c-simulations
```
```bash
cd /c/c-simulations
```

## Buiding the Project

1. Navigate to your projects directory in **MSYS2 MinGW 64-bit terminal**:

```bash
cd /c/c-simulations
```

2. Run the makefile:

```bash
make PROJECT=eulerian_fluid_sim
# The command will follow the pattern: make PROJECT={simulation folder name}
```

3. Navigate to the 'build' directory:

```bash
cd build
```

4. Run the simulation:

```bash
./eulurian_fluid_sim_app.exe
# The command will follow the pattern: ./{simulation folder name}_app.exe
```

5. Clean the project:

```bash
make clean
```
