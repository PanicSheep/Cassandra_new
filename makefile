CC=g++
CFLAGS=-c -Wall -std=c++11 -I googletest/googletest/test/ src/
LDFLAGS=

SOURCES_SRC=$(wildcard src/*.cpp)
SOURCES_TEST=$(wildcard test/*.cpp)
OBJECTS_SRC=$(SOURCES_SRC:src/%.cpp=obj/%.o)
OBJECTS_TEST=$(SOURCES_TEST:test/%.cpp=obj/%.o)

$(OBJECTS_SRC): obj/%.o : src/%.cpp
	$(CC) $(CFLAGS) -c $< -o $@
	@echo "Compiled "$<" successfully!"
    
$(OBJECTS_TEST): obj/%.o : test/%.cpp
	$(CC) $(CFLAGS) -c $< -o $@
	@echo "Compiled "$<" successfully!"

bin/test_flip_loop: obj/test_flip_loop.o obj/flip_loop.o
	$(CC) $(LDFLAGS) $< -o $@

test: bin/test_flip_loop
	./bin/test_flip_loop

all:

clean :
	rm -f *.o
	rm -f *~
	rm -f src/*~
	rm -f test/*~
