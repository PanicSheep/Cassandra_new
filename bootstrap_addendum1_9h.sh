#!/bin/bash

# Generate next positions (??? Min)
#for e in {1..60}
#do
#	./bin/posgen -rnd -next -i pos/rnd/e$((e)).psc -o pos/rnd/e$((e-1))_next.psc
#done

# Sove positions 0 - 13 (3 Min)
for e in {0..13}
do
	echo "Solving pos/rnd/e$((e)).psc"
	./bin/solver -f pos/rnd/e$((e)).psc
	echo "Solving pos/rnd/e$((e))_next.psc"
	./bin/solver -f pos/rnd/e$((e))_next.psc
	
	echo "Generating matrices from pos/rnd/e${e}.psc"
	./bin/positiontomatrix -pos pos/rnd/e$((e)).psc -mat matvec/rnd -vec matvec/rnd
	echo "Generating matrices from pos/rnd/e${e}_next.psc"
	./bin/positiontomatrix -pos pos/rnd/e$((e))_next.psc -mat matvec/rnd -vec matvec/rnd
done

# Fit weights (???? Min)
./bin/weightfitter -p e0 e0_next e1 e1_next e2 e2_next e3 e3_next e4 e4_next -m matvec/rnd -v matvec/rnd -w matvec/range0
for e in {1..3}
do
	./bin/weightfitter -p e$((3*e+0)) e$((3*e+0))_next e$((3*e+1)) e$((3*e+1))_next e$((3*e+2)) e$((3*e+2))_next e$((3*e+3)) e$((3*e+3))_next e$((3*e+4)) e$((3*e+4))_next -m matvec/rnd -v matvec/rnd -w matvec/range$((e)) -w0 matvec/range$((e-1))
done

for e in {0..11}
do
	rm matvec/rnd/e$((e))* # TODO: Remove when disc space is big enough
done

# Solve positions range 4 - 15 (??? Min)
for e in {4..15}
do
	for f in {2..4}
	do
		echo "Solving pos/rnd/e$((3*e+f)).psc"
		./bin/solver -f pos/rnd/e$((3*e+f)).psc -d 7 -noskip
		./bin/solver -f pos/rnd/e$((3*e+f))_next.psc -d 7 -noskip
		./bin/positiontomatrix -pos pos/rnd/e$((3*e+f)).psc -mat matvec/rnd -vec matvec/rnd
		./bin/positiontomatrix -pos pos/rnd/e$((3*e+f))_next.psc -mat matvec/rnd -vec matvec/rnd
	done

	./bin/weightfitter -p e$((3*e+0)) e$((3*e+0))_next e$((3*e+1)) e$((3*e+1))_next e$((3*e+2)) e$((3*e+2))_next e$((3*e+3)) e$((3*e+3))_next e$((3*e+4)) e$((3*e+4))_next -m matvec/rnd -v matvec/rnd -w matvec/range$((e)) -w0 matvec/range$((e-1))
	
	rm matvec/rnd/e$((3*e+0))* # TODO: Remove when disc space is big enough
	rm matvec/rnd/e$((3*e+1))* # TODO: Remove when disc space is big enough
	rm matvec/rnd/e$((3*e+2))* # TODO: Remove when disc space is big enough
done

rm matvec/rnd/* # TODO: Remove when disc space is big enough
echo "All done!"