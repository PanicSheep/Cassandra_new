CC=g++
CFLAGS=-c -Wall -std=c++11
LDFLAGS=

all: test

test: bin\test_flip_loop
    
bin\test_flip_loop: obj\test_flip_loop.o
	$(CC) $(LDFLAGS) $< -o $@

%.o: %.cpp
	$(CC) $(CFLAGS) -o ..\obj\$@ $<


clean :
	rm -f *.o
	rm -f *~
	rm -f src/*~
	rm -f test/*~
