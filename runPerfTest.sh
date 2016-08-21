#!/bin/bash

# DO NOT modify any files while this is running

threads=1
if [[ -n "$1" ]]
then
	threads="$1"
else
	echo "Need argument for how many threads!"
	exit 1
fi

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

rollouts=2500  # Can adjust

echo "rollouts $rollouts" >> $tempPerfConfig
echo "trials 20" >> $tempPerfConfig
echo "warmupTrials 5" >> $tempPerfConfig
echo "superko 1" >> $tempPerfConfig
echo "hashBuckets 1000" >> $tempPerfConfig
echo "threads $threads" >> $tempPerfConfig
echo "respect -1" >> $tempPerfConfig  # Never resigns, otherwise would mess up times
#echo "raveV 10" >> $tempPerfConfig

./ergo -t -C $tempPerfConfig

read -r average < $rawPerfFile  # Should just be one line

rm $rawPerfFile
rm $tempPerfConfig

echo "$average"

exit 0
