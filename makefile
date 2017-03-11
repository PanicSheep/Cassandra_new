CXX=g++

HEADER=flip_loop.h

flip_loop: flip_loop.o $(HEADER)
	$(CXX) -o $@ $^

main: main.o $(HEADER)
	$(CXX) -o $@ $^

%.o : %.cpp $(HEADER)
	$(CXX) -c $< -o $@

.PHONY : test
test : flip_loop
	./$<

.PHONY : all
all : flip_loop main

.PHONY : clean
clean :
	rm -f *.o
	rm -f *~
	rm -f src/*~
	rm -f test/*~
