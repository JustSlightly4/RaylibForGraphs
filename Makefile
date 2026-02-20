CXX = g++
CXXFLAGS = -Wall -std=c++17 -I./LinuxLibrary
LIBS = ./LinuxLibrary/libraylib.a -lGL -lm -lpthread -ldl -lrt -lX11

ifeq ($(OS),Windows_NT)
    # Windows settings
    CXX = g++
	CXXFLAGS = -Wall -std=c++17 -I./WindowsLibrary
	LIBS = ./WindowsLibrary/libraylib.a -lopengl32 -lgdi32 -lwinmm
else
    # Linux / macOS settings
    CXX = g++
	CXXFLAGS = -Wall -std=c++17 -I./LinuxLibrary
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