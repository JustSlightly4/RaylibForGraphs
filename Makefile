# ==========================================
# Raylib Project Makefile (Desktop & Web)
# ==========================================

# 1. Project Settings
TARGET = main
SRC = UIDrawer.cpp Buttons.cpp main.cpp
RAYLIB_PATH = ./raylib/src

# 2. Compiler Settings
CXX = g++
CXXFLAGS = -Wall -std=c++17 -I$(RAYLIB_PATH)

# 3. OS Detection & Desktop Flags
ifeq ($(OS),Windows_NT)
    # Windows Desktop (MinGW)
    RM = del /f /q
    LDFLAGS = $(RAYLIB_PATH)/libraylib.a -lopengl32 -lgdi32 -lwinmm -lshell32 -luser32
    EXE = $(TARGET).exe
else
    # Linux / WSL
    RM = rm -f
    LDFLAGS = $(RAYLIB_PATH)/libraylib.a -lGL -lm -lpthread -ldl -lrt -lX11
    EXE = $(TARGET)
endif

# 4. Web/Emscripten Settings
EM_CXX = em++
RAYLIB_LIB_WEB = $(RAYLIB_PATH)/libraylib.web.a
WEB_OUTPUT = index.html

# Emscripten specific flags
# ASYNCIFY is required for the main loop to work in browsers
EM_FLAGS = -s USE_GLFW=3 -s ASYNCIFY -DPLATFORM_WEB \
           -s FORCE_FILESYSTEM=1 \
           --shell-file $(RAYLIB_PATH)/shell.html \
           --preload-file textures@/textures \
           --preload-file fonts@/fonts

# ==========================================
# Rules
# ==========================================

# Default rule: Build Desktop version
all: $(EXE)

# Build Desktop Executable
$(EXE): $(SRC)
	@if not exist $(RAYLIB_PATH)\libraylib.a ($(MAKE) -C $(RAYLIB_PATH) PLATFORM=PLATFORM_DESKTOP)
	$(CXX) $(CXXFLAGS) $(SRC) -o $(EXE) $(LDFLAGS)

# Build Web version
# Run this with: mingw32-make web
web: $(SRC)
	@if not exist $(RAYLIB_LIB_WEB) ($(MAKE) -C $(RAYLIB_PATH) PLATFORM=PLATFORM_WEB -B)
	$(EM_CXX) $(CXXFLAGS) $(SRC) -o $(WEB_OUTPUT) $(RAYLIB_LIB_WEB) $(EM_FLAGS)

# Local Server: Run the web version locally
serve:
	python -m http.server 8080

# Clean project files
clean:
ifeq ($(OS),Windows_NT)
	-$(RM) $(TARGET).exe index.html index.js index.wasm index.data 2>nul || exit 0
else
	$(RM) $(TARGET) index.html index.js index.wasm index.data
endif

# This cleans internal Raylib object files but leaves ALL final outputs intact
clean-internal:
	$(MAKE) -C $(RAYLIB_PATH) clean PLATFORM=PLATFORM_DESKTOP
	$(MAKE) -C $(RAYLIB_PATH) clean PLATFORM=PLATFORM_WEB
	@echo "Internal build files removed. Final outputs (.exe, .html, .wasm) are preserved."

# Deep clean (including Raylib libraries)
clean-all: clean
	$(MAKE) -C $(RAYLIB_PATH) clean

.PHONY: all clean clean-all web serve clean-internal