CC=g++
CFLAGS=-c -Wall -std=c++11
LDFLAGS=

all: test

test: bin\test_flip_loop
    
bin\test_flip_loop: obj\test_flip_loop.o
	$(CC) $(LDFLAGS) $< -o $@

obj\%.o: src\%.cpp
	$(CC) $(CFLAGS) -o $@ $<
	
obj\%.o: test\%.cpp
	$(CC) $(CFLAGS) -o $@ $<

clean :
	rm -f *.o
	rm -f *~
	rm -f src/*~
	rm -f test/*~
