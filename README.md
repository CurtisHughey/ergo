https://travis-ci.org/CurtisHughey/ergo.svg?branch=master

Author: Curtis Hughey

ergo is a small MCTS implementation of Go.  The README just specifies how everything is run - for actual, interesting details, check the Wiki here: https://github.com/CurtisHughey/ergo/wiki.

---

To install (basically just creating a couple of directories):
  > ./install.sh

---
  
While there is a makefile, it is encouraged to use build.sh to compile (basically, because it's easier to pass in command line options, and it will automatically clean).  To build:
  > ./build.sh
  
If no arguments are provided, a 19x19 board by default will be compiled.  To compile with a different size board (in this case 5x5:
  > ./build.sh -d 5
  
Boards can be 1x1 through 19x19, square only (no rectangular boards at this time).

You can also pass in -v, which will make the program ready to be run with Valgrind (this is used for some correctness tests).

---

To run the program:
  > ./ergo
  
There are loads of flags and arguments to pass in; you can see all of them with:
  > ./ergo -h
  
You can choose to play player vs. player, player vs. computer, run a GTP session, etc.  

---

To run basic correctness tests (unit test suite, memory tests, some regression tests):
  > ./runTests.sh
  
---

To run a full suite of regression tests, courtesy of CGTC and GoGui (see regTests/cgtc/README and regTests/gogui/README.html) respectively:
  > ./runRegTests.sh
  
The main output is regTests/results/index.html.

---

To run this program on the KGS server:
  > ./runKgsGtp.sh 19
  
This will run the bot, called Xantos, on the KGS Computer Go room, with a 19x19 board (again, can specify from 1 to 19, although if there is no argument it will default to 19).  You can kill it with ctrl-c.

---

Three separate instances of ergo can be run at one time: on the KGS server, regression tests, and interactive/unit tests/etc.  For the first two, the ergo executable is copied to kgsGtp or regTests respectively, and for the last one it is just in the main directory.  In each of those directories, there is a configs directory.  Passing in the -C flag to ergo allows you to specify a configuration file (you can specify number of rollouts, threads, number of hash buckets, etc).  Refer to configParser.c's getDefaultConfig for all the options.  To specify an option in a config file, for example number of rollouts:
  > rollouts 50000

Command line arguments will override the value given in a configuration file, if provided.

---

To run performance tests (basically, times a UCT iteration):
  > ./runPerfTests.sh
  
Passing in a -s flag will save the average runtime to perf/performance.txt.  You can view a graph of performance over the commits at perf/performance.png.

---

To profile the program with callgrind, and see it on kcachegrind:
  > ./profile.sh

---

If you don't like the random numbers I generated on my machine for the Zobrist data:
  > cd zobristData; ./randNumGenerator.sh
  
---
  
