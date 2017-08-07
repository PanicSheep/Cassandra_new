mkdir pos\rnd

:: Generate random positions (2 Min)
FOR /L %%e IN (0,1,49) DO x64\Release\posgen.exe -rnd -o pos/rnd/e%%e_1M.psc -n 1000000 -e %%e
FOR /L %%e IN (50,1,60) DO x64\Release\posgen.exe -all -o pos/rnd/e%%e_all.psc -e %%e