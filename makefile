CC=g++
CFLAGS=-c -Wall -std=c++11
LDFLAGS=

SOURCES_SRC=$(wildcard src/*.cpp)
SOURCES_TEST=$(wildcard test/*.cpp)
OBJECTS=$(SOURCES:$(SRCDIR)/%.cpp=$(OBJDIR)/%.o)

all: test

test: bin\test_flip_loop
    
bin\test_flip_loop: obj\test_flip_loop.o
	$(CC) $(LDFLAGS) $< -o $@
	
obj\flip_loop.o: src\flip_loop.cpp
	$(CC) $(CFLAGS) $< -o $@

obj\test_flip_loop.o: test\test_flip_loop.cpp
	$(CC) $(CFLAGS) $< -o $@


clean :
	rm -f *.o
	rm -f *~
	rm -f src/*~
	rm -f test/*~
