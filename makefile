CC=g++
CFLAGS=-c -Wall
LDFLAGS=
SOURCES=main.cpp flip_loop.cpp
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=main

all: $(SOURCES) $(EXECUTABLE)
    
$(EXECUTABLE): $(OBJECTS) 
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@

clean :
	rm -f *.o
	rm -f *~
	rm -f src/*~
	rm -f test/*~
