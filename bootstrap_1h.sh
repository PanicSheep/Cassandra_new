#!/bin/bash

# Generate random positions (2 Min)
#for e in {0..49}
#do
#	./bin/posgen -rnd -o pos/rnd/e${e}_1M.psc -n 1000000 -e $e
#done
#
#for e in {50..60}
#do
#	./bin/posgen -all -o pos/rnd/e${e}_all.psc -e $e
#done

# Generate default weights
./bin/positiontomatrix -pos pos/rnd/e0_1M.psc -mat matvec/rnd -vec matvec/rnd
for e in {0..15}
do
	./bin/weightfitter -p e0_1M -m matvec/rnd -v matvec/rnd -w matvec/range${e} -maxit 0
done

# Generate next positions (??? Min)
#for e in {1..49}
#do
#	./bin/posgen -rnd -next -i pos/rnd/e${e}_1M.psc -o pos/rnd/e$((e-1))_next_1M.psc
#done

#for e in {50..60}
#do
#	./bin/posgen -all -next -i pos/rnd/e${e}_all.psc -o pos/rnd/e$((e-1))_next_all.psc -e $e
#done

# Sove positions 0 - 10 (3 Min)
for e in {0..10}
do
	echo Solving pos/rnd/e${e}_1M.psc
	./bin/solver -f pos/rnd/e${e}_1M.psc
done

# Print score histogram positions 0 - 10
for e in {0..10}
do	
	./bin/positionmanager -hist -i pos/rnd/e${e}_1M.psc
done

# Generate matrices and vectors (1 Min)
for e in {0..10}
do	
	./bin/positiontomatrix -pos pos/rnd/e${e}_1M.psc -mat matvec/rnd -vec matvec/rnd
done

# Fit weights (2 Min)
./bin/weightfitter -p e0_1M e1_1M e2_1M e3_1M e4_1M -m matvec/rnd -v matvec/rnd -w matvec/range0
./bin/weightfitter -p e3_1M e4_1M e5_1M e6_1M e7_1M -m matvec/rnd -v matvec/rnd -w matvec/range1 -w0 matvec/range0
./bin/weightfitter -p e6_1M e7_1M e8_1M e9_1M e10_1M -m matvec/rnd -v matvec/rnd -w matvec/range2 -w0 matvec/range1

# Solve positions range 3 - 15 (??? Min)
for e in {3..15}
do
	echo Solving pos/rnd/e$((3*e+2))_1M.psc
	./bin/solver -f pos/rnd/e$((3*e+2))_1M.psc -d 4
	./bin/positiontomatrix -pos pos/rnd/e$((3*e+2))_1M.psc -mat matvec/rnd -vec matvec/rnd

	echo Solving pos/rnd/e$((3*e+3))_1M.psc
	./bin/solver -f pos/rnd/e$((3*e+3))_1M.psc -d 4
	./bin/positiontomatrix -pos pos/rnd/e$((3*e+3))_1M.psc -mat matvec/rnd -vec matvec/rnd

	echo Solving pos/rnd/e$((3*e+4))_1M.psc
	./bin/solver -f pos/rnd/e$((3*e+4))_1M.psc -d 4
	./bin/positiontomatrix -pos pos/rnd/e$((3*e+4))_1M.psc -mat matvec/rnd -vec matvec/rnd

	./bin/weightfitter -p e$((3*e+0))_1M e$((3*e+1))_1M e$((3*e+2))_1M e$((3*e+3))_1M e$((3*e+4))_1M -m matvec/rnd -v matvec/rnd -w matvec/range${e} -w0 matvec/range$((e-1))
done

