# Project Settings
TARGET = main
SRC = UIDrawer.cpp Buttons.cpp main.cpp
RAYLIB_PATH = ./raylib/src
RAYLIB_LIB = $(RAYLIB_PATH)/libraylib.a

# Compiler Settings
CXX = g++
CXXFLAGS = -Wall -std=c++17 -I$(RAYLIB_PATH)

# OS Detection
ifeq ($(OS),Windows_NT)
    # Windows-specific flags (MinGW)
    LDFLAGS = $(RAYLIB_LIB) -lopengl32 -lgdi32 -lwinmm
    EXE = $(TARGET).exe
else
    # Linux/WSL-specific flags
    # Added X11-related flags often required for Raylib
    LDFLAGS = $(RAYLIB_LIB) -lGL -lm -lpthread -ldl -lrt -lX11 -lXrandr -lXinerama -lXcursor -lXi
    EXE = $(TARGET)
endif

# Default Rule
all: $(RAYLIB_LIB) $(EXE)

# Build the project executable
$(EXE): $(SRC)
	$(CXX) $(CXXFLAGS) $(SRC) -o $(EXE) $(LDFLAGS)

# Build Raylib if it doesn't exist (ensures Linux build on Linux)
$(RAYLIB_LIB):
	@echo "Building Raylib library..."
	$(MAKE) -C $(RAYLIB_PATH) PLATFORM=PLATFORM_DESKTOP

# Clean project files
clean:
	rm -f $(TARGET) $(TARGET).exe

# Clean everything including Raylib
clean-all: clean
	$(MAKE) -C $(RAYLIB_PATH) clean

.PHONY: all clean clean-all