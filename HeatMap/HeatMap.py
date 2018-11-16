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

def MagicFormula1(data, alpha, beta, gamma):
    D = data[:,0]
    d = data[:,1]
    E = data[:,2]
    return (np.exp(alpha*d)+beta) * (D-d)**gamma

def MagicFormula2(data, alpha, beta, gamma, delta, epsilon):
    D = data[:,0]
    d = data[:,1]
    E = data[:,2]
    return (np.exp(alpha*d)+beta) * (delta*E+epsilon) * (D-d)**gamma

measurements = [] # D, d, E, stddev

start = timeit.default_timer()
for r in range(0, 7):
    filename = "C:\\Users\\dohof_000\\Documents\\GitHub\\Cassandra_new.git\\pos\\rndAllDepthScore\\range%s.pos" % r
    puzzles = [puzzle for puzzle in Deserialize_Puzzles(filename) if type(puzzle) is PuzzleAllDepthScore]
    
    #scores = np.array([p.score[:] for p in puzzles])
    #score_heuristic = np.array(scores[:,0])
    #score_heuristic = np.array([p.score[0] for p in puzzles])
    #score_exact = np.array([p.score[p.EmptyCount()] for p in puzzles])
        
    #plt.suptitle("range%s.pos" % r)
    
    #plot = plt.subplot(2, 2, 1)
    #plot.xaxis.tick_top()
    #plot.xaxis.set_label_position("top")
    #plt.xlabel('Exact score')
    #plt.hist(score_exact, 129, (-64, 64))
    
    #plt.subplot(2, 2, 3)
    #plt.hist2d(score_exact, score_heuristic, bins=[129,129], range=[[-64,64],[-64,64]], cmap='hot')
    
    #plot = plt.subplot(2, 2, 4)
    #plot.yaxis.tick_right()
    #plot.yaxis.set_label_position("right")
    #plt.ylabel('Estimated score')
    #plt.hist(score_heuristic, 129, (-64, 64), orientation="horizontal")
    
    #plt.text(10,200,'$\sigma(error)=$%s' % np.std(np.subtract(score_heuristic, score_exact), ddof=1))
    #plt.text(10,215,'$\#(score=0)$ : %s' % sum(1 for x in score_heuristic if x == 0))

    ##plt.savefig("C:\\Users\\dohof_000\\Documents\\GitHub\\Cassandra_new.git\\pos\\test\\histogram\\e%s.png" % empties)
    #plt.show()
    #plt.clf()

    for e in set([p.EmptyCount() for p in puzzles]):
        scores = np.array([p.score[:] for p in puzzles if p.EmptyCount() == e])
        measurements.extend([[D, d, e, np.std(np.subtract(scores[:,d], scores[:,D]), ddof=1)] for d in range(0, e) for D in range(d+1, e+1)])

measurements = np.array(measurements)

x = measurements[:,0:3]
y = measurements[:,3]
popt, pcov = curve_fit(MagicFormula1, x, y, (-0.2,1,0.3))

y_fit1 = MagicFormula1(x, popt[0], popt[1], popt[2])

# residual sum of squares
ss_res = np.sum((y - y_fit1) ** 2)

# total sum of squares
ss_tot = np.sum((y - np.mean(y)) ** 2)

# r-squared
r2 = 1 - (ss_res / ss_tot)

print(popt)
print(r2)

popt, pcov = curve_fit(MagicFormula2, x, y, (-0.2,1,0.3,0,2))

y_fit2 = MagicFormula2(x, popt[0], popt[1], popt[2], popt[3], popt[4])

# residual sum of squares
ss_res = np.sum((y - y_fit2) ** 2)

# total sum of squares
ss_tot = np.sum((y - np.mean(y)) ** 2)

# r-squared
r2 = 1 - (ss_res / ss_tot)

print(popt)
print(r2)

end = timeit.default_timer()
print(end - start)

#for e in range(4, 16)
#    fig = plt.figure()
#    ax = fig.add_subplot(111, projection='3d')
#    ax.set_xlabel('depth')
#    ax.set_ylabel('empties')
#    ax.set_zlabel('stddev')
#    tmp = measurements[measurements[:,0] == measurements[:,2]]
#    X = tmp[:,1]
#    Y = tmp[:,2]
#    Z = tmp[:,3]
#    surf = ax.scatter(X, Y, Z)
#    ax.scatter(X, Y, y_fit1[measurements[:,0] == measurements[:,2]])
#    ax.scatter(X, Y, y_fit2[measurements[:,0] == measurements[:,2]])
#    plt.show()
    
fig = plt.figure()
ax = fig.add_subplot(111, projection='3d')
ax.set_xlabel('Depth')
ax.set_ylabel('depth')
ax.set_zlabel('stddev')
for e in range(4, 16):
    tmp = measurements[measurements[:,2] == e]
    ax.scatter(tmp[:,0], tmp[:,1], tmp[:,3])
plt.show()


fig = plt.figure()
ax = fig.add_subplot(111, projection='3d')
ax.set_xlabel('Depth')
ax.set_ylabel('empties')
ax.set_zlabel('stddev')
tmp = measurements[measurements[:,0] == measurements[:,2]]
X = tmp[:,0]
Y = tmp[:,2]
Z = tmp[:,3]
surf = ax.scatter(X, Y, Z)
ax.scatter(X, Y, y_fit1[measurements[:,0] == measurements[:,2]])
ax.scatter(X, Y, y_fit2[measurements[:,0] == measurements[:,2]])
plt.show()


fig = plt.figure()
ax = fig.add_subplot(111, projection='3d')
ax.set_xlabel('depth')
ax.set_ylabel('empties')
ax.set_zlabel('stddev')
tmp = measurements[measurements[:,0] == measurements[:,2]]
X = tmp[:,1]
Y = tmp[:,2]
Z = tmp[:,3]
surf = ax.scatter(X, Y, Z)
ax.scatter(X, Y, y_fi1[measurements[:,0] == measurements[:,2]])
ax.scatter(X, Y, y_fit2[measurements[:,0] == measurements[:,2]])
plt.show()