CXX=c++
LDFLAGS=-lcunit

CPPUNITLDFLAGS=-lcppunit
GOOGLELDFLAGS=-lgtest -lgtest_main

SRCDIR=src
TESTDIR=test
INC=-I$(SRCDIR) -I$(TESTDIR) -I$(HOME)/include

HEADER=flip_loop.h

flip_loop: flip_loop.o $(HEADER)
	$(CXX) -o $@ $^ $(INC)

%.o : $(SRCDIR)/%.cc $(HEADER)
	$(CXX) -c $< -o $@ $(INC)
%.o : $(TESTDIR)/%.cc $(HEADER)
	$(CXX) -c $< -o $@ $(INC)

.PHONY : test
test : flip_loop
	./$<

.PHONY : googletest
googletest : fibonacci-googletests
	./$< --gtest_output="xml:TestResults.xml"

.PHONY : all
all : flip_loop test googletest

.PHONY : clean
clean :
	rm -f TestResults.xml
	rm -f *.o
	rm -f *~
	rm -f src/*~
	rm -f test/*~
