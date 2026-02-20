CXX = g++
CXXFLAGS = -Wall -std=c++17 -I./LinuxLibrary
LIBS = ./LinuxLibrary/libraylib.a -lGL -lm -lpthread -ldl -lrt -lX11

TARGET = main
SRC = main.cpp

all: $(TARGET)

$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) $(SRC) -o $(TARGET) $(LIBS)

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(TARGET)