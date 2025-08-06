# How to Contribute

## Setup

### Prerequisites

To get started, please follow the setup instructions in the [README.md](README.md). To summarize, you should have:
- MSYS2
- Make
- SDL2

### Compiling & Running the Code

As stated in the [README.md](README.md), compiling and running the simulations requires the following steps:
- Cloning the repo
- Building the project with `make PROJECT={simulation_folder_name}
- Running the project by moving to the build directory and executing the .exe

## Style and Structure

While this project has a somewhat flexible style, please adhere to the following guidelines:
- File Organization: Each simulation should have its own directory.  Under that directory there should be src and include folders, housing the `.c` and `.h` files respectively.
- Naming Conventions:
  - SCREAMING_SNAKE_CASE for constants
  - snake_case for functions
  - camelCase for variables
  - PascalCase for structures

If there is a style guideline that you are not sure about, please look at previous code to get an idea of how to style the code.

## What to Contribute

While pretty much anything within the scope of this repo can be added, if you are not sure what you want to add, here are some ideas:
- Bug fixes
- Performance improvements
- Adding new simulations

## Closing Thoughts

Thank you for considering contributing! If you have any questions, feel free to open an issue.

