# This is good enough for now, boy I suck at python

import numpy as np
import matplotlib.pyplot as plt
import itertools as it
import operator as op
import sys
from matplotlib.ticker import MaxNLocator

perfGraph = "performance.png"

inputFileName = sys.argv[1]

title = "Performance"

data = np.genfromtxt(inputFileName, delimiter=',', dtype=None, autostrip=True)
parsedData =  [(d[0], d[1]) for d in list(data)] 


finalData = []
# This is all so stupid
for key, group in it.groupby(parsedData, key=lambda x: x[0]):
	group, copy = it.tee(group)
	finalData.append((key, sum(y for _,y in group)/len(list(copy))))

print finalData

fig, ax = plt.subplots()

plt.title(title)
plt.xlabel("Commit Number")
plt.ylabel("Average Performance (millis)")

ax.xaxis.set_major_locator(MaxNLocator(integer=True))

xs, ys = zip(*finalData)
plt.plot(xs, ys, "-o")

# legend = ax.legend(loc='upper right', shadow=True, title="Type")

plt.savefig(perfGraph)