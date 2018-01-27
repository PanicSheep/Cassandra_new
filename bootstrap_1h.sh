#!/bin/bash

# Generate random positions (2 Min)
for e in {0..49}
do
	./bin/posgen -rnd -o pos/rnd/e$((e)).psc -n 1000000 -e $e
done

#for e in {50..60}
#do
#	./bin/posgen -all -o pos/rnd/e$((e)).psc -e $((e))
#done

# Generate default weights
for e in {0..15}
do
	./bin/weightfitter -w matvec/range$((e)) -default
done 

# Sove positions 0 - 10 (3 Min)
for e in {0..10}
do
	echo "Solving pos/rnd/e$((e)).psc"
	./bin/solver -f pos/rnd/e$((e)).psc
	./bin/positionmanager -hist -i pos/rnd/e$((e)).psc >> hist.log
done

# Generate matrices and vectors (1 Min)
for e in {0..10}
do
	./bin/positiontomatrix -pos pos/rnd/e$((e)).psc -mat matvec/rnd -vec matvec/rnd
done

# Fit weights (2 Min)
./bin/weightfitter -p e0 e1 e2 e3 e4 -m matvec/rnd -v matvec/rnd -w matvec/range0
./bin/weightfitter -p e3 e4 e5 e6 e7 -m matvec/rnd -v matvec/rnd -w matvec/range1 -w0 matvec/range0
./bin/weightfitter -p e6 e7 e8 e9 e10 -m matvec/rnd -v matvec/rnd -w matvec/range2 -w0 matvec/range1

for e in {0..8}
do
	rm matvec/rnd/e$((e))* # TODO: Remove when disc space is big enough
done

# Solve positions range 3 - 15 (??? Min)
for e in {3..15}
do
	echo "Solving pos/rnd/e$((3*e+2)).psc"
	./bin/solver -f pos/rnd/e$((3*e+2)).psc -d 4
	./bin/positiontomatrix -pos pos/rnd/e$((3*e+2)).psc -mat matvec/rnd -vec matvec/rnd

	echo "Solving pos/rnd/e$((3*e+3)).psc"
	./bin/solver -f pos/rnd/e$((3*e+3)).psc -d 4
	./bin/positiontomatrix -pos pos/rnd/e$((3*e+3)).psc -mat matvec/rnd -vec matvec/rnd

	echo "Solving pos/rnd/e$((3*e+4)).psc"
	./bin/solver -f pos/rnd/e$((3*e+4)).psc -d 4
	./bin/positiontomatrix -pos pos/rnd/e$((3*e+4)).psc -mat matvec/rnd -vec matvec/rnd

	./bin/weightfitter -p e$((3*e+0)) e$((3*e+1)) e$((3*e+2)) e$((3*e+3)) e$((3*e+4)) -m matvec/rnd -v matvec/rnd -w matvec/range$((e)) -w0 matvec/range$((e-1))
	
	rm matvec/rnd/e$((3*e+0))* # TODO: Remove when disc space is big enough
	rm matvec/rnd/e$((3*e+1))* # TODO: Remove when disc space is big enough
	rm matvec/rnd/e$((3*e+2))* # TODO: Remove when disc space is big enough
done

rm matvec/rnd/* # TODO: Remove when disc space is big enough