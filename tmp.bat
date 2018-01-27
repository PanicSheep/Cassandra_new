FOR /L %%e IN (14,1,19) DO bin\solver.exe -f pos\rnd\e%%e_1M.psc -noskip
FOR /L %%e IN (15,1,20) DO bin\solver.exe -f pos\rnd\e%%e_next_1M.psc -noskip