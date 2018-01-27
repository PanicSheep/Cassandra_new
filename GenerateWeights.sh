#!/bin/bash

# Sove positions 0 - 13 (3 Min)
for e in {0..13}
do
	echo "Generating matrices from pos/rnd/e${e}_1M.psc"
	./bin/positiontomatrix -pos pos/rnd/e$((e))_1M.psc -mat matvec/rnd -vec matvec/rnd
	echo "Generating matrices from pos/rnd/e${e+1}_next_1M.psc"
	./bin/positiontomatrix -pos pos/rnd/e$((e+1))_next_1M.psc -mat matvec/rnd -vec matvec/rnd
done

# Fit weights (???? Min)
./bin/weightfitter -p e0_1M e1_next_1M e1_1M e2_next_1M e2_1M e3_next_1M e3_1M e4_next_1M e4_1M e5_next_1M -m matvec/rnd -v matvec/rnd -w matvec/range0 # has no -w0
for e in {0..15}
do
	./bin/weightfitter -p e$((3*e+0))_1M e$((3*e+1))_next_1M e$((3*e+1))_1M e$((3*e+2))_next_1M e$((3*e+2))_1M e$((3*e+3))_next_1M e$((3*e+3))_1M e$((3*e+4))_next_1M e$((3*e+4))_1M e$((3*e+5))_next_1M -m matvec/rnd -v matvec/rnd -w matvec/range$((e)) -w0 matvec/range$((e-1))
done

echo "All done!"