#!/bin/bash

# DO NOT modify any files while this is running

save=0
if [[ -n "$1" ]]
then
	if [[ "$1" -eq "-s" ]]
	then
		save=1
	else
		echo "Command argument $1 not recognized"
	fi
fi

perfFile="perf/performance.txt"
rawPerfFile="perf/rawPerformance.txt"
rm -f $rawPerfFile

tempPerfConfig="configs/tempPerfConfig.txt"
rm -f $tempPerfConfig

./build.sh -d 19 &>/dev/null

if [[ $? -ne 0 ]]
then
	echo "Failed to compile"
	exit 1
fi

echo "rollouts 1000" >> $tempPerfConfig
echo "trials 20" >> $tempPerfConfig
echo "warmupTrials 3" >> $tempPerfConfig
echo "superko 1" >> $tempPerfConfig
echo "hashBuckets 1000" >> $tempPerfConfig

./ergo -t -C $tempPerfConfig

read -r average < $rawPerfFile  # Should just be one line

if [[ $save -eq 1 ]]
then
	gitCommitData=$(git shortlog -s)
	set -- $gitCommitData
	echo "$1,$average" >> $perfFile
fi

python perf/processPerf.py perf/performance.txt

rm $rawPerfFile
rm $tempPerfConfig