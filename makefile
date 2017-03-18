GTEST_DIR = googletest/googletest
CC=g++
CFLAGS=-c -Wall -std=c++11 -march=native -O3 -I $(GTEST_DIR) -I $(GTEST_DIR)/include -I src
LDFLAGS= -pthread

# All Google test headers.
GTEST_HEADERS = $(GTEST_DIR)/include/gtest/*.h \
		$(GTEST_DIR)/include/gtest/internal/*.h

SOURCES_SRC=$(wildcard src/*.cpp)
SOURCES_TEST=$(wildcard test/*.cpp)
OBJECTS_SRC=$(SOURCES_SRC:src/%.cpp=obj/%.o)
OBJECTS_TEST=$(SOURCES_TEST:test/%.cpp=obj/%.o)


# Builds gtest.a gtest_main.a
GTEST_SRCS_ = $(GTEST_DIR)/src/*.cc $(GTEST_DIR)/src/*.h $(GTEST_HEADERS)

gtest-all.o : $(GTEST_SRCS_)
	$(CC) $(CFLAGS) $(GTEST_DIR)/src/gtest-all.cc

gtest_main.o : $(GTEST_SRCS_)
	$(CC) $(CFLAGS) $(GTEST_DIR)/src/gtest_main.cc

gtest.a : gtest-all.o
	ar rvs $@ $^

gtest_main.a : gtest-all.o gtest_main.o
	ar rvs $@ $^



$(OBJECTS_SRC): obj/%.o : src/%.cpp
	$(CC) $(CFLAGS) $< -o $@
	@echo "Compiled "$<" successfully!"
    
$(OBJECTS_TEST): obj/%.o : test/%.cpp $(GTEST_HEADERS)
	$(CC) $(CFLAGS) gtest-all.o $< -o $@
	@echo "Compiled "$<" successfully!"

bin/test_flip_loop: obj/test_flip_loop.o obj/flip_loop.o gtest-all.o
	$(CC) $(LDFLAGS) obj/test_flip_loop.o obj/flip_loop.o gtest-all.o -o $@
	
bin/test_macros_hell: obj/test_macros_hell.o gtest-all.o
	$(CC) $(LDFLAGS) obj/test_macros_hell.o gtest-all.o -o $@

bin/test_flip_fast: obj/test_flip_fast.o obj/flip_loop.o obj/flip_fast.o gtest-all.o
	$(CC) $(LDFLAGS) obj/test_flip_fast.o obj/flip_loop.o obj/flip_fast.o gtest-all.o -o $@

test: bin/test_flip_loop bin/test_macros_hell bin/test_flip_fast
	./bin/test_flip_loop
	./bin/test_macros_hell
	./bin/test_flip_fast

all:

clean :
	rm -f *.o
	rm -f *~
	rm -f src/*~
	rm -f test/*~
