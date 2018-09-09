import numpy as np
from scipy.stats import norm
from scipy.optimize import curve_fit
import matplotlib.pyplot as plt
import matplotlib.mlab as mlab
from mpl_toolkits.mplot3d import Axes3D
import struct
import array
import timeit

from Puzzles import *

#data = np.fromfile("C:\\Users\\dohof_000\\Documents\\GitHub\\Cassandra_new.git\\weights\\range0_D4.w", 'f4')
#plt.hist(data, 129, (min(data), max(data)))
#plt.show()
#plt.clf()

#data = np.fromfile("C:\\Users\\dohof_000\\Documents\\GitHub\\Cassandra_new.git\\weights\\range0_D5.w", 'f4')
#plt.hist(data, 129, (min(data), max(data)))
#plt.show()
#plt.clf()

#data = np.fromfile("C:\\Users\\dohof_000\\Documents\\GitHub\\Cassandra_new.git\\weights\\range0_D7.w", 'f4')
#plt.hist(data, 129, (min(data), max(data)))
#plt.show()
#plt.clf()

#data = np.fromfile("C:\\Users\\dohof_000\\Documents\\GitHub\\Cassandra_new.git\\weights\\range0_Ep.w", 'f4')
#plt.hist(data, 129, (min(data), max(data)))
#plt.show()
#plt.clf()

#data = np.fromfile("C:\\Users\\dohof_000\\Documents\\GitHub\\Cassandra_new.git\\weights\\range0_B5.w", 'f4')
#plt.hist(data, 129, (min(data), max(data)))
#plt.show()
#plt.clf()

def MagicFormula(D, d, E, alpha, beta, gamma, delta, epsilon):
    return (np.exp(alpha*d)+beta) * (delta*E+epsilon) * (D-d)**gamma

Measurements = [] # D, d, E, stddev

start = timeit.default_timer()
for empties in range(1, 20):
    filename = "C:\\Users\\dohof_000\\Documents\\GitHub\\Cassandra_new.git\\pos\\EvalSet\\e%s.pos" % empties
    scores = np.array([puzzle.score[:empties+1] for puzzle in Deserialize_Puzzles(filename) if type(puzzle) is PuzzleAllDepthScore])
                
    exact_score = scores[:,empties]
    heuristic_score = scores[:,0]
        
    plt.suptitle("e%s.pos" % empties)
    
    plot = plt.subplot(2, 2, 1)
    plot.xaxis.tick_top()
    plot.xaxis.set_label_position("top")
    plt.xlabel('Exact score')
    plt.hist(exact_score, 129, (-64, 64))
    
    plt.subplot(2, 2, 3)
    plt.hist2d(exact_score, heuristic_score, bins=[129,129], range=[[-64,64],[-64,64]], cmap='hot')
    
    plot = plt.subplot(2, 2, 4)
    plot.yaxis.tick_right()
    plot.yaxis.set_label_position("right")
    plt.ylabel('Estimated score')
    plt.hist(heuristic_score, 129, (-64, 64), orientation="horizontal")
    
    plt.text(10,200,'$\sigma(error)=$%s' % np.std(np.subtract(heuristic_score, exact_score), ddof=1))
    plt.text(10,215,'$score=0$ : %s' % sum(1 for x in heuristic_score if x == 0))

    plt.savefig("C:\\Users\\dohof_000\\Documents\\GitHub\\Cassandra_new.git\\pos\\EvalSet\\histogram\\e%s.png" % empties)
    plt.show()
    plt.clf()
    
    Measurements.extend([[D, d, empties, np.std(np.subtract(scores[:,d], scores[:,D]), ddof=1)] for d in range(0, empties+1) for D in range(d, empties+1)])
    
#scipy.optimize.curve_fit(MagicFormula, )

Measurements = np.array(Measurements)
end = timeit.default_timer()
print(stop - end)

fig = plt.figure()
ax = fig.add_subplot(111, projection='3d')
ax.set_xlabel('depth')
ax.set_ylabel('empties')
ax.set_zlabel('stddev')
tmp = Measurements[Measurements[:,0] == Measurements[:,2]]
X = tmp[:,1]
Y = tmp[:,2]
Z = tmp[:,3]
surf = ax.scatter(X, Y, Z)
plt.show()


fig = plt.figure()
ax = fig.add_subplot(111, projection='3d')
ax.set_xlabel('Depth')
ax.set_ylabel('depth')
ax.set_zlabel('stddev')
tmp = Measurements[Measurements[:,0] == Measurements[:,2]]
X = tmp[:,0]
Y = tmp[:,1]
Z = tmp[:,3]
surf = ax.scatter(X, Y, Z)
plt.show()


fig = plt.figure()
ax = fig.add_subplot(111, projection='3d')
ax.set_xlabel('Depth')
ax.set_ylabel('empties')
ax.set_zlabel('stddev')
tmp = Measurements[Measurements[:,0] == Measurements[:,2]]
X = tmp[:,0]
Y = tmp[:,2]
Z = tmp[:,3]
surf = ax.scatter(X, Y, Z)
plt.show()


fig = plt.figure()
ax = fig.add_subplot(111, projection='3d')
ax.set_xlabel('depth')
ax.set_ylabel('empties')
ax.set_zlabel('stddev')
tmp = Measurements[Measurements[:,0] == Measurements[:,2]]
X = tmp[:,1]
Y = tmp[:,2]
Z = tmp[:,3]
surf = ax.scatter(X, Y, Z)
plt.show()