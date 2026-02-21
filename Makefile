CXX = g++
CXXFLAGS = -Wall -std=c++17 -I./LinuxLibrary UIDrawer.cpp Buttons.cpp
LIBS = ./LinuxLibrary/libraylib.a -lGL -lm -lpthread -ldl -lrt -lX11

ifeq ($(OS),Windows_NT)

    ifneq (,$(findstring MINGW,$(UNAME_S)))
        # MinGW
        CXX = g++
        CXXFLAGS = -Wall -std=c++17 -I./WindowsLibrary UIDrawer.cpp Buttons.cpp
        LIBS = ./WindowsLibrary/libraylib.a -lopengl32 -lgdi32 -lwinmm

    else ifneq (,$(findstring CYGWIN,$(UNAME_S)))
        # Cygwin
        CXX = g++
        CXXFLAGS = -Wall -std=c++17 -I./WindowsLibrary UIDrawer.cpp Buttons.cpp
        LIBS = ./WindowsLibrary/libraylib.a -lopengl32 -lgdi32 -lwinmm

    endif

else
    # Linux / macOS
    CXX = g++
    CXXFLAGS = -Wall -std=c++17 -I./LinuxLibrary UIDrawer.cpp Buttons.cpp
    LIBS = ./LinuxLibrary/libraylib.a -lGL -lm -lpthread -ldl -lrt -lX11
endif

TARGET = main
SRC = main.cpp

all: $(TARGET)

$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) $(SRC) -o $(TARGET) $(LIBS)

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(TARGET)