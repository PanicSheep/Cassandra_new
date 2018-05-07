#!/bin/bash

mkdir pos/rnd

# Generate random positions (3 Min)
for e in {0..49}
do
	./bin/Generator -rnd -o pos/rnd/e$((e)).pos -n 1000000 -e $e
done

#for e in {50..60}
do
	./bin/Generator -all -o pos/rnd/e$((e)).pos -e $((e))
done