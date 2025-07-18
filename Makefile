# --- Project Configuration ---
# Define the build directory
BUILD_DIR = build

# Set the PROJECT variable to choose which simulation to build.
# For example, to build the ray tracer, run: make PROJECT=ray_tracer_sim
# Default to eulerian_fluid_sim if not specified
PROJECT ?= eulerian_fluid_sim

# Dynamically set SRC_DIR, INCLUDE_DIR, and TARGET based on the selected PROJECT
SRC_DIR = $(PROJECT)/src
INCLUDE_DIR = $(PROJECT)/include
TARGET = $(PROJECT)_app # Naming the executable based on the project

# Path to your manually included runtime DLLs
RUNTIME_DLLS_SRC_DIR = lib

# Define SDL DLLs needed for runtime
SDL_RUNTIME_DLL_NAMES = SDL2.dll

# Full paths to the runtime DLLs in the build directory (where they will be copied)
SDL_RUNTIME_DLLS_BUILD = $(patsubst %,$(BUILD_DIR)/%,$(SDL_RUNTIME_DLL_NAMES))

# List all your .c source files dynamically based on the PROJECT
SRCS = $(wildcard $(SRC_DIR)/*.c)

# Automatically generate the list of object files from source files
OBJS = $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(SRCS))


CC = gcc

# Standard CFLAGS:
CFLAGS = -Wall -Wextra -I$(INCLUDE_DIR) -g

# SDL2 specific CFLAGS and LIBS
SDL_CFLAGS = $(shell pkg-config --cflags sdl2)
SDL_LIBS = $(shell pkg-config --libs sdl2)

# Combine all CFLAGS and LIBS
CFLAGS += $(SDL_CFLAGS) $(SDL_TTF_CFLAGS)
LDFLAGS = $(SDL_LIBS) $(SDL_TTF_LIBS)

.PHONY: all
all: $(BUILD_DIR)/$(TARGET)

# Rule to create the build directory if it doesn't exist
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# Rule to link the object files into the final executable
$(BUILD_DIR)/$(TARGET): $(OBJS) $(SDL_RUNTIME_DLLS_BUILD) | $(BUILD_DIR)
	$(CC) $(OBJS) -o $@ $(LDFLAGS)

# Rule to compile each .c file into a .o object file
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Rule to copy SDL DLLs from lib/ to build/
$(BUILD_DIR)/%.dll: $(RUNTIME_DLLS_SRC_DIR)/%.dll | $(BUILD_DIR)
	cp $< $@

# Rule to clean up generated files
.PHONY: clean
clean:
	rm -rf $(BUILD_DIR)
