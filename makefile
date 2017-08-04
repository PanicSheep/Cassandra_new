GTEST_DIR = googletest/googletest
CC=g++
CFLAGS=-c -fopenmp -Wall -std=c++11 -march=native -O3 -I $(GTEST_DIR) -I $(GTEST_DIR)/include -I src
LDFLAGS= -fopenmp -lpthread -std=c++11

# All Google test headers.
GTEST_HEADERS = $(GTEST_DIR)/include/gtest/*.h \
		$(GTEST_DIR)/include/gtest/internal/*.h

SOURCES_SRC=$(wildcard src/*.cpp)
SOURCES_TEST=$(wildcard test/*.cpp)
OBJECTS_SRC=$(SOURCES_SRC:src/%.cpp=obj/%.o)
OBJECTS_TEST=$(SOURCES_TEST:test/%.cpp=obj/%.o)


# Builds gtest.a gtest_main.a
GTEST_SRCS_ = $(GTEST_DIR)/src/*.cc $(GTEST_DIR)/src/*.h $(GTEST_HEADERS)

obj/gtest-all.o : $(GTEST_SRCS_)
	$(CC) $(CFLAGS) $(GTEST_DIR)/src/gtest-all.cc -o $@

gtest_main.o : $(GTEST_SRCS_)
	$(CC) $(CFLAGS) $(GTEST_DIR)/src/gtest_main.cc

gtest.a : obj/gtest-all.o
	ar rvs $@ $^

gtest_main.a : obj/gtest-all.o gtest_main.o
	ar rvs $@ $^

$(OBJECTS_SRC): obj/%.o : src/%.cpp
	$(CC) $(CFLAGS) $< -o $@
	@echo "\033[1;33mCompiled\033[0m \033[0;33m"$<"\033[0m \033[1;33msuccessfully!\033[0m"
    
$(OBJECTS_TEST): obj/%.o : test/%.cpp $(GTEST_HEADERS) obj/gtest-all.o
	$(CC) $(CFLAGS) obj/gtest-all.o $< -o $@
	@echo "\033[1;33mCompiled\033[0m \033[0;33m"$<"\033[0m \033[1;33msuccessfully!\033[0m"

bin/test_array2D: obj/test_array2D.o obj/gtest-all.o
	$(CC) $(LDFLAGS) $^ -o $@
	./bin/test_array2D

bin/test_vecvec: obj/test_vecvec.o obj/gtest-all.o
	$(CC) $(LDFLAGS) $^ -o $@
	./bin/test_vecvec

bin/test_matrixCSR_Grid: obj/test_matrixCSR_Grid.o obj/gtest-all.o
	$(CC) $(LDFLAGS) $^ -o $@
	./bin/test_matrixCSR_Grid

bin/test_flip_loop: obj/test_flip_loop.o obj/flip_loop.o obj/gtest-all.o
	$(CC) $(LDFLAGS) $^ -o $@
	./bin/test_flip_loop
	
bin/test_macros_hell: obj/test_macros_hell.o obj/gtest-all.o
	$(CC) $(LDFLAGS) $^ -o $@
	./bin/test_macros_hell

bin/test_utility: obj/test_utility.o obj/utility.o obj/gtest-all.o
	$(CC) $(LDFLAGS) $^ -o $@
	./bin/test_utility

bin/test_configfile: obj/test_configfile.o obj/configfile.o obj/gtest-all.o
	$(CC) $(LDFLAGS) $^ -o $@
	./bin/test_configfile

bin/test_path: obj/test_path.o obj/configfile.o obj/path.o obj/gtest-all.o
	$(CC) $(LDFLAGS) $^ -o $@
	./bin/test_path

bin/test_datamanipulation: obj/test_datamanipulation.o obj/gtest-all.o
	$(CC) $(LDFLAGS) $^ -o $@
	./bin/test_datamanipulation

bin/test_flip_fast: obj/test_flip_fast.o obj/flip_loop.o obj/flip_fast.o obj/helpers.o obj/gtest-all.o
	$(CC) $(LDFLAGS) $^ -o $@
	./bin/test_flip_fast

bin/test_count_last_flip: obj/test_count_last_flip.o obj/flip_fast.o obj/count_last_flip.o obj/helpers.o obj/gtest-all.o
	$(CC) $(LDFLAGS) $^ -o $@
	./bin/test_count_last_flip

bin/test_possiblemoves: obj/test_possiblemoves.o obj/possiblemoves.o obj/possiblemoves_sse2.o obj/possiblemoves_avx2.o obj/possiblemoves_avx512.o obj/flip_fast.o obj/helpers.o obj/gtest-all.o
	$(CC) $(LDFLAGS) $^ -o $@
	./bin/test_possiblemoves

bin/test_position: obj/test_position.o obj/position.o obj/possiblemoves.o obj/possiblemoves_sse2.o obj/possiblemoves_avx2.o obj/possiblemoves_avx512.o obj/flip_fast.o obj/utility.o obj/gtest-all.o
	$(CC) $(LDFLAGS) $^ -o $@
	./bin/test_position
	
bin/test_generatepositions: obj/test_generatepositions.o obj/generatepositions.o obj/position.o obj/possiblemoves.o obj/possiblemoves_sse2.o obj/possiblemoves_avx2.o obj/possiblemoves_avx512.o obj/flip_fast.o obj/utility.o obj/gtest-all.o
	$(CC) $(LDFLAGS) $^ -o $@
	./bin/test_generatepositions

bin/test_perft: obj/test_perft.o obj/perft_basic.o obj/position.o obj/possiblemoves.o obj/possiblemoves_sse2.o obj/possiblemoves_avx2.o obj/possiblemoves_avx512.o obj/flip_fast.o obj/utility.o obj/gtest-all.o
	$(CC) $(LDFLAGS) $^ -o $@
	./bin/test_perft
	
bin/perft_haswell: obj/perft_haswell.o obj/perft_basic.o obj/position.o obj/possiblemoves.o obj/possiblemoves_sse2.o obj/possiblemoves_avx2.o obj/possiblemoves_avx512.o obj/flip_fast.o obj/utility.o obj/gtest-all.o
	$(CC) $(LDFLAGS) $^ -o $@

bin/test_line: obj/test_line.o obj/position.o obj/possiblemoves.o obj/possiblemoves_sse2.o obj/possiblemoves_avx2.o obj/possiblemoves_avx512.o obj/flip_fast.o obj/utility.o obj/gtest-all.o
	$(CC) $(LDFLAGS) $^ -o $@
	./bin/test_line

bin/test_pattern: obj/test_pattern.o obj/configfile.o obj/pattern.o obj/position.o obj/possiblemoves.o obj/possiblemoves_sse2.o obj/possiblemoves_avx2.o obj/possiblemoves_avx512.o obj/flip_fast.o obj/configfile.o obj/gtest-all.o
	$(CC) $(LDFLAGS) $^ -o $@
	./bin/test_pattern

bin/test_game: obj/test_game.o obj/configfile.o obj/pattern.o obj/position.o obj/game_endgame_negamax.o obj/game_endgame_alphabeta.o obj/game_endgame_pvs.o obj/game_midgame_pvs.o obj/game_pvs.o obj/count_last_flip.o obj/possiblemoves.o obj/possiblemoves_sse2.o obj/possiblemoves_avx2.o obj/possiblemoves_avx512.o obj/flip_fast.o obj/utility.o obj/hashtable.o obj/move.o obj/gtest-all.o
	$(CC) $(LDFLAGS) $^ -o $@
	./bin/test_game

bin/test_positiontomatrix: obj/test_positiontomatrix.o obj/configfile.o obj/pattern.o obj/position.o obj/possiblemoves.o obj/possiblemoves_sse2.o obj/possiblemoves_avx2.o obj/possiblemoves_avx512.o obj/flip_fast.o obj/utility.o obj/generatepositions.o obj/gtest-all.o
	$(CC) $(LDFLAGS) $^ -o $@
	./bin/test_positiontomatrix

bin/test_weightfitter: obj/test_weightfitter.o obj/configfile.o obj/generatepositions.o obj/pattern.o obj/position.o obj/game_endgame_negamax.o obj/game_endgame_alphabeta.o obj/game_endgame_pvs.o obj/game_midgame_pvs.o obj/game_pvs.o obj/count_last_flip.o obj/possiblemoves.o obj/possiblemoves_sse2.o obj/possiblemoves_avx2.o obj/possiblemoves_avx512.o obj/flip_fast.o obj/utility.o obj/hashtable.o obj/move.o obj/gtest-all.o
	$(CC) $(LDFLAGS) $^ -o $@
	./bin/test_weightfitter

bin/solver: obj/solver.o obj/configfile.o obj/pattern.o obj/position.o obj/game_endgame_negamax.o obj/game_endgame_alphabeta.o obj/game_endgame_pvs.o obj/game_midgame_pvs.o obj/game_pvs.o obj/count_last_flip.o obj/possiblemoves.o obj/possiblemoves_sse2.o obj/possiblemoves_avx2.o obj/possiblemoves_avx512.o obj/flip_fast.o obj/utility.o obj/hashtable.o obj/move.o obj/configfile.o obj/gtest-all.o
	$(CC) $(LDFLAGS) $^ -o $@

bin/posgen: obj/posgen.o obj/position.o obj/possiblemoves.o obj/possiblemoves_sse2.o obj/possiblemoves_avx2.o obj/possiblemoves_avx512.o obj/flip_fast.o obj/utility.o obj/generatepositions.o obj/gtest-all.o
	$(CC) $(LDFLAGS) $^ -o $@

bin/positionmanager: obj/positionmanager.o obj/position.o obj/possiblemoves.o obj/possiblemoves_sse2.o obj/possiblemoves_avx2.o obj/possiblemoves_avx512.o obj/flip_fast.o obj/utility.o obj/generatepositions.o obj/gtest-all.o
	$(CC) $(LDFLAGS) $^ -o $@

bin/positiontomatrix: obj/positiontomatrix.o obj/configfile.o obj/pattern.o obj/position.o obj/possiblemoves.o obj/possiblemoves_sse2.o obj/possiblemoves_avx2.o obj/possiblemoves_avx512.o obj/flip_fast.o obj/utility.o obj/generatepositions.o obj/gtest-all.o
	$(CC) $(LDFLAGS) $^ -o $@

bin/weightfitter: obj/weightfitter.o obj/utility.o obj/configfile.o obj/pattern.o obj/position.o obj/possiblemoves.o obj/possiblemoves_sse2.o obj/possiblemoves_avx2.o obj/possiblemoves_avx512.o obj/flip_fast.o obj/configfile.o obj/gtest-all.o
	$(CC) $(LDFLAGS) $^ -o $@

bin/PCfitter: obj/PCfitter.o obj/configfile.o obj/pattern.o obj/position.o obj/game_endgame_negamax.o obj/game_endgame_alphabeta.o obj/game_endgame_pvs.o obj/game_midgame_pvs.o obj/game_pvs.o obj/count_last_flip.o obj/possiblemoves.o obj/possiblemoves_sse2.o obj/possiblemoves_avx2.o obj/possiblemoves_avx512.o obj/flip_fast.o obj/utility.o obj/hashtable.o obj/move.o obj/configfile.o obj/generatepositions.o obj/gtest-all.o
	$(CC) $(LDFLAGS) $^ -o $@

.PHONY: test
test: bin/test_array2D bin/test_vecvec bin/test_matrixCSR_Grid bin/test_flip_loop bin/test_macros_hell bin/test_utility bin/test_configfile bin/test_path bin/test_datamanipulation bin/test_flip_fast bin/test_count_last_flip bin/test_possiblemoves bin/test_position bin/test_generatepositions bin/test_perft bin/test_line bin/test_pattern bin/test_game bin/test_positiontomatrix bin/test_weightfitter

.PHONY: all
all: bin/perft_haswell bin/solver bin/posgen bin/positionmanager bin/positiontomatrix bin/weightfitter bin/PCfitter

.PHONY: clean
clean :
	rm -f -r obj/*.o
	rm -f src/*~
	rm -f test/*~