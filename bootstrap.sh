#!/bin/bash

for e in {0..49}
do
   ./bin/posgen -rnd -o pos/rnd/e${e}_1M.psc -n 1000000 -e $e
done

for e in {50..60}
do
   ./bin/posgen -all -o pos/rnd/e${e}_all.psc -e $e
done
