CC=g++
CFLAGS=-c -Wall -std=c++11
LDFLAGS=
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
