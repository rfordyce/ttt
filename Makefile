CC=g++
CFLAGS=-c -Wall -fopenmp -std=c++11
LDFLAGS=-fopenmp
SOURCES=ttt.cpp
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=m_ttt

all: $(SOURCES) $(EXECUTABLE)
	
$(EXECUTABLE): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm *.o $(EXECUTABLE)
