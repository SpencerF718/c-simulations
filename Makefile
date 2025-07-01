# --- Project Configuration ---
# Define the name of the executable that will be created
TARGET = sdl_gui_app
# Define the build directory
BUILD_DIR = build
# Define the base directory for source files
SRC_DIR = eulerian_fluid_sim/src
# Define the base directory for include files
INCLUDE_DIR = eulerian_fluid_sim/include

# Path to your manually included runtime DLLs
RUNTIME_DLLS_SRC_DIR = lib

# Define SDL DLLs needed for runtime
SDL_RUNTIME_DLL_NAMES = SDL2.dll
# SDL_RUNTIME_DLL_NAMES += SDL2_ttf.dll

# Full paths to the runtime DLLs in the source directory (not directly used by rule, but helpful for context)
# SDL_RUNTIME_DLLS = $(patsubst %,$(RUNTIME_DLLS_SRC_DIR)/%,$(SDL_RUNTIME_DLL_NAMES))

# Full paths to the runtime DLLs in the build directory (where they will be copied)
SDL_RUNTIME_DLLS_BUILD = $(patsubst %,$(BUILD_DIR)/%,$(SDL_RUNTIME_DLL_NAMES))

# --- Source and Object Files ---
# List all your .c source files
SRCS = $(SRC_DIR)/main_sdl_fluid.c \
       $(SRC_DIR)/fluid_logic.c

# Automatically generate the list of object files from source files
OBJS = $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(SRCS))


# --- Compiler and Linker Flags ---
CC = gcc

# Standard CFLAGS:
CFLAGS = -Wall -Wextra -I$(INCLUDE_DIR) -g

# SDL2 specific CFLAGS and LIBS
SDL_CFLAGS = $(shell pkg-config --cflags sdl2)
SDL_LIBS = $(shell pkg-config --libs sdl2)

# Combine all CFLAGS and LIBS
CFLAGS += $(SDL_CFLAGS) $(SDL_TTF_CFLAGS)
LDFLAGS = $(SDL_LIBS) $(SDL_TTF_LIBS)

# --- Makefile Rules ---

# Default target: builds the executable
.PHONY: all
all: $(BUILD_DIR)/$(TARGET)

# Rule to create the build directory if it doesn't exist
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# Rule to link the object files into the final executable
$(BUILD_DIR)/$(TARGET): $(OBJS) $(SDL_RUNTIME_DLLS_BUILD) | $(BUILD_DIR)
	$(CC) $(OBJS) -o $@ $(LDFLAGS)

# Rule to compile each .c file into a .o object file
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c $(INCLUDE_DIR)/fluid_logic.h | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Corrected Rule to copy SDL DLLs from lib/ to build/
$(BUILD_DIR)/%.dll: $(RUNTIME_DLLS_SRC_DIR)/%.dll | $(BUILD_DIR) # <--- THIS LINE IS CORRECTED
	cp $< $@

# Rule to clean up generated files
.PHONY: clean
clean:
	rm -rf $(BUILD_DIR)
