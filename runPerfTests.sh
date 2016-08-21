#!/bin/bash

# DO NOT modify any files while this is running

perfFile="perf/performance.txt"

threads=7  # Optimal on my system

save=0
if [[ -n "$1" ]]
then
	if [[ "$1" -eq "-s" ]]
	then
		save=1
	else
		echo "Command argument $1 not recognized"
		exit 1
	fi
fi

average="$(./runPerfTest.sh $threads | tail -1)"
echo "Average Performance: $average"

if [[ $save -eq 1 ]]
then
	gitCommitData=$(git shortlog -s)
	set -- $gitCommitData
	echo "$1,${average}" >> "$perfFile"
	python perf/processPerf.py "${perfFile}"  # Makes the new image
fi

exit 0
