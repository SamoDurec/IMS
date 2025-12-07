CXX = g++
CXXFLAGS = -std=c++11 -Wall -I/usr/local/include

# Linkujeme priamo statickú knižnicu
LDFLAGS = -L/usr/local/lib -lsimlib -lm

TARGET = ski_sim
SOURCES = lyziar.cpp generator.cpp main.cpp
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
