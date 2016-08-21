# Ripped from processPerf.py

import numpy as np
import matplotlib.pyplot as plt
import itertools as it
import operator as op
import sys
from matplotlib.ticker import MaxNLocator

threadPerfGraph = "perf/threadPerformance.png"

inputFileName = sys.argv[1]

title = "Thread Performance"

data = np.genfromtxt(inputFileName, delimiter=',', dtype=None, autostrip=True)
parsedData =  [(d[0], d[1]) for d in list(data)] 

fig, ax = plt.subplots()

plt.title(title)
plt.xlabel("Threads")
plt.ylabel("Average Time (millis)")

ax.xaxis.set_major_locator(MaxNLocator(integer=True))

xs, ys = zip(*parsedData)
plt.plot(xs, ys, "-o")

# legend = ax.legend(loc='upper right', shadow=True, title="Type")

plt.savefig(threadPerfGraph)
