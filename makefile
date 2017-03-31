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
	$(CC) $(LDFLAGS) $^ -o $@
	./bin/test_flip_loop
	
bin/test_macros_hell: obj/test_macros_hell.o gtest-all.o
	$(CC) $(LDFLAGS) $^ -o $@
	./bin/test_macros_hell

bin/test_utility: obj/test_utility.o obj/utility.o gtest-all.o
	$(CC) $(LDFLAGS) $^ -o $@
	./bin/test_utility

bin/test_configfile: obj/test_configfile.o obj/configfile.o gtest-all.o
	$(CC) $(LDFLAGS) $^ -o $@
	./bin/test_configfile

bin/test_path: obj/test_path.o obj/configfile.o obj/path.o gtest-all.o
	$(CC) $(LDFLAGS) $^ -o $@
	./bin/test_path

bin/test_datamanipulation: obj/test_datamanipulation.o gtest-all.o
	$(CC) $(LDFLAGS) $^ -o $@
	./bin/test_datamanipulation

bin/test_flip_fast: obj/test_flip_fast.o obj/flip_loop.o obj/flip_fast.o obj/helpers.o gtest-all.o
	$(CC) $(LDFLAGS) $^ -o $@
	./bin/test_flip_fast

bin/test_count_last_flip: obj/test_count_last_flip.o obj/flip_fast.o obj/count_last_flip.o obj/helpers.o gtest-all.o
	$(CC) $(LDFLAGS) $^ -o $@
	./bin/test_count_last_flip

bin/test_possiblemoves: obj/test_possiblemoves.o obj/possiblemoves.o obj/possiblemoves_sse2.o obj/possiblemoves_avx2.o obj/possiblemoves_avx512.o obj/flip_fast.o obj/helpers.o gtest-all.o
	$(CC) $(LDFLAGS) $^ -o $@
	./bin/test_possiblemoves

bin/test_position: obj/test_position.o obj/position.o obj/possiblemoves.o obj/possiblemoves_sse2.o obj/possiblemoves_avx2.o obj/possiblemoves_avx512.o obj/flip_fast.o obj/utility.o gtest-all.o
	$(CC) $(LDFLAGS) $^ -o $@
	./bin/test_position
	
bin/test_generatepositions: obj/test_generatepositions.o obj/generatepositions.o obj/position.o obj/possiblemoves.o obj/possiblemoves_sse2.o obj/possiblemoves_avx2.o obj/possiblemoves_avx512.o obj/flip_fast.o obj/utility.o gtest-all.o
	$(CC) $(LDFLAGS) $^ -o $@
	./bin/test_generatepositions

bin/test_perft: obj/test_perft.o obj/perft_basic.o obj/position.o obj/possiblemoves.o obj/possiblemoves_sse2.o obj/possiblemoves_avx2.o obj/possiblemoves_avx512.o obj/flip_fast.o obj/utility.o gtest-all.o
	$(CC) $(LDFLAGS) $^ -o $@
	./bin/test_perft

bin/test_game: obj/test_game.o obj/position.o obj/game_endgame_negamax.o obj/game_endgame_alphabeta.o obj/count_last_flip.o obj/possiblemoves.o obj/possiblemoves_sse2.o obj/possiblemoves_avx2.o obj/possiblemoves_avx512.o obj/flip_fast.o obj/utility.o gtest-all.o
	$(CC) $(LDFLAGS) $^ -o $@
	./bin/test_game

.PHONY: test
test: bin/test_flip_loop bin/test_macros_hell bin/test_utility bin/test_configfile bin/test_path bin/test_datamanipulation bin/test_flip_fast bin/test_count_last_flip bin/test_possiblemoves bin/test_position bin/test_generatepositions bin/test_perft bin/test_game

.PHONY: all
all:

.PHONY: clean
clean :
	rm -f *.o
	rm -f *~
	rm -f src/*~
	rm -f test/*~
