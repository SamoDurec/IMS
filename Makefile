CXX = g++
CXXFLAGS = -std=c++11 -Wall -I. -I/Users/samueldurec/Downloads/simlib/src

# Linkujeme priamo statickú knižnicu
LDFLAGS = /Users/samueldurec/Downloads/simlib/src/simlib.a

TARGET = ski_sim
SOURCES = main.cpp skier.cpp
OBJECTS = $(SOURCES:.cpp=.o)

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CXX) $(OBJECTS) $(LDFLAGS) -o $(TARGET)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(TARGET) *.o

run: $(TARGET)
	./$(TARGET)

test: clean all run

.PHONY: all clean run test
