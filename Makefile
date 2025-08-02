BUILD_DIR = build

# Default to eulerian_fluid_sim if not specified
PROJECT ?= eulerian_fluid_sim

# Dynamically set SRC_DIR, INCLUDE_DIR, and TARGET based on the selected PROJECT
SRC_DIR = $(PROJECT)/src
INCLUDE_DIR = $(PROJECT)/include
TARGET = $(PROJECT)_app

RUNTIME_DLLS_SRC_DIR = lib

SDL_RUNTIME_DLL_NAMES = SDL2.dll

# Full paths to the runtime DLLs in the build directory
SDL_RUNTIME_DLLS_BUILD = $(patsubst %,$(BUILD_DIR)/%,$(SDL_RUNTIME_DLL_NAMES))

SRCS = $(wildcard $(SRC_DIR)/*.c)

OBJS = $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(SRCS))

CC = gcc

CFLAGS = -Wall -Wextra -I$(INCLUDE_DIR) -g

SDL_CFLAGS = $(shell pkg-config --cflags sdl2)
SDL_LIBS = $(shell pkg-config --libs sdl2)

CFLAGS += $(SDL_CFLAGS) $(SDL_TTF_CFLAGS)
LDFLAGS = $(SDL_LIBS) $(SDL_TTF_LIBS)

.PHONY: all
all: $(BUILD_DIR)/$(TARGET)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(BUILD_DIR)/$(TARGET): $(OBJS) $(SDL_RUNTIME_DLLS_BUILD) | $(BUILD_DIR)
	$(CC) $(OBJS) -o $@ $(LDFLAGS)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/%.dll: $(RUNTIME_DLLS_SRC_DIR)/%.dll | $(BUILD_DIR)
	cp $< $@

.PHONY: clean
clean:
	rm -rf $(BUILD_DIR)
