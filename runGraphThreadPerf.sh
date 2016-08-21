#!/bin/bash

threadPerfFile="perf/threadPerf.txt"
> "$threadPerfFile"  # Clears it

for ((i=1; i<=12; i++));
do
	echo "Threads: $i"
	average="$(./runPerfTest.sh $i | tail -1)"
	echo "Average: $average"
	echo "$i,${average}" >> "$threadPerfFile"
done

python perf/processThreadPerf.py "${threadPerfFile}"
