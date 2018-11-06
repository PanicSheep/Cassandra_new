import numpy
from scipy.stats import norm
import matplotlib.pyplot as plt
import matplotlib.mlab as mlab
import struct
import array

from Puzzles import *

for r in range(0, 6):
    filename = "C:\\Users\\dohof_000\\Documents\\GitHub\\Cassandra_new.git\\pos\\test\\range%s.pos" % r
    scores = [puzzle.score for puzzle in Deserialize_Puzzles(filename) if type(puzzle) is PuzzleScore and puzzle.IsSolved()]

    n, bins, patches = plt.hist(scores, 65, (-64, 64))

    (mu, sigma) = norm.fit(scores)
    y = mlab.normpdf(bins, mu, sigma);

    plt.title("range%s.pos" % r)
    plt.xlabel('Score')
    plt.ylabel('Count')
    plt.text(-7, 10000, '$\mu=$%s\n$\sigma=$%s' % (round(mu,1), round(sigma,1)))
    plt.plot(bins, y * len(scores) * 2, 'r--', linewidth=2)

    plt.show()
    #plt.savefig("C:\\Users\\dohof_000\\Documents\\GitHub\\Cassandra_new.git\\pos\\rnd\\histogram\\e%s.png" % empties)
    plt.clf()
