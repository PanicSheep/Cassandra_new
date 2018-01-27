#!/bin/bash

# Generate random positions (2 Min)
for e in {0..49}
do
	./bin/posgen -rnd -o pos/rnd/e${e}_1M.psc -n 1000000 -e $e
done

for e in {50..60}
do
	./bin/posgen -all -o pos/rnd/e${e}_all.psc -e $e
done

# Generate default weights
for e in {0..15}
do
	./bin/weightfitter -w matvec/range${e} -default
done

# Generate next positions (??? Min)
for e in {1..49}
do
	./bin/posgen -rnd -next -i pos/rnd/e${e}_1M.psc -o pos/rnd/e$((e-1))_next_1M.psc
done

for e in {50..60}
do
	./bin/posgen -all -next -i pos/rnd/e${e}_all.psc -o pos/rnd/e$((e-1))_next_all.psc -e $e
done

# Sove positions 0 - 13 (3 Min)
for e in {0..19}
do
	echo "Solving pos/rnd/e${e}_1M.psc"
	./bin/solver -f pos/rnd/e${e}_1M.psc
	echo "Solving pos/rnd/e${e}_next_1M.psc"
	./bin/solver -f pos/rnd/e$((e+1))_next_1M.psc
	
	echo "Generating matrices from pos/rnd/e${e}_1M.psc"
	./bin/positiontomatrix -pos pos/rnd/e$((e))_1M.psc -mat matvec/rnd -vec matvec/rnd
	echo "Generating matrices from pos/rnd/e${e+1}_next_1M.psc"
	./bin/positiontomatrix -pos pos/rnd/e$((e+1))_next_1M.psc -mat matvec/rnd -vec matvec/rnd
done

# Fit weights (???? Min)
./bin/weightfitter -p e0_1M e1_next_1M e1_1M e2_next_1M e2_1M e3_next_1M e3_1M e4_next_1M e4_1M e5_next_1M -m matvec/rnd -v matvec/rnd -w matvec/range0
for e in {0..5}
do
	./bin/weightfitter -p e$((3*e+0))_1M e$((3*e+1))_next_1M e$((3*e+1))_1M e$((3*e+2))_next_1M e$((3*e+2))_1M e$((3*e+3))_next_1M e$((3*e+3))_1M e$((3*e+4))_next_1M e$((3*e+4))_1M e$((3*e+5))_next_1M -m matvec/rnd -v matvec/rnd -w matvec/range$((e)) -w0 matvec/range$((e-1))
done

# Solve positions range 3 - 15 (??? Min)
for e in {6..10}
do
	echo "Solving pos/rnd/e$((3*e+2))_1M.psc"
	./bin/solver -f pos/rnd/e$((3*e+2))_1M.psc -d 7
	./bin/solver -f pos/rnd/e$((3*e+3))_next_1M.psc -d 7
	./bin/positiontomatrix -pos pos/rnd/e$((3*e+2))_1M.psc -mat matvec/rnd -vec matvec/rnd
	./bin/positiontomatrix -pos pos/rnd/e$((3*e+3))_next_1M.psc -mat matvec/rnd -vec matvec/rnd

	echo "Solving pos/rnd/e$((3*e+3))_1M.psc"
	./bin/solver -f pos/rnd/e$((3*e+3))_1M.psc -d 7
	./bin/solver -f pos/rnd/e$((3*e+4))_next_1M.psc -d 7
	./bin/positiontomatrix -pos pos/rnd/e$((3*e+3))_1M.psc -mat matvec/rnd -vec matvec/rnd
	./bin/positiontomatrix -pos pos/rnd/e$((3*e+4))_next_1M.psc -mat matvec/rnd -vec matvec/rnd

	echo "Solving pos/rnd/e$((3*e+4))_1M.psc"
	./bin/solver -f pos/rnd/e$((3*e+4))_1M.psc -d 7
	./bin/solver -f pos/rnd/e$((3*e+5))_next_1M.psc -d 7
	./bin/positiontomatrix -pos pos/rnd/e$((3*e+4))_1M.psc -mat matvec/rnd -vec matvec/rnd
	./bin/positiontomatrix -pos pos/rnd/e$((3*e+5))_next_1M.psc -mat matvec/rnd -vec matvec/rnd

	./bin/weightfitter -p e$((3*e+0))_1M e$((3*e+1))_next_1M e$((3*e+1))_1M e$((3*e+2))_next_1M e$((3*e+2))_1M e$((3*e+3))_next_1M e$((3*e+3))_1M e$((3*e+4))_next_1M e$((3*e+4))_1M e$((3*e+5))_next_1M -m matvec/rnd -v matvec/rnd -w matvec/range${e} -w0 matvec/range$((e-1))
done
echo "All done!"