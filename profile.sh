#!/bin/bash

tempConfig="configs/tempConfig.txt"

./build.sh -v -d 3 &>/dev/null  # -v so it can be tracked on valgrind, 3x3 so it's fast
if [[ $? -ne 0 ]]
then
	echo "Failed to compile"
	exit 1
fi

echo "rollouts 1000" >> $tempConfig  # Not too much
echo "superko 1" >> $tempConfig
echo "threads 1" >> $tempConfig  # Callgrind melts down if we're multithreaded :(

valgrind --tool=callgrind ./ergo -x -C $tempConfig &>/dev/null

callgrindFile="$(ls -t callgrind* | head -1)"  # Gets most recent

kcachegrind $callgrindFile &>/dev/null  # Opens it

rm $tempConfig
rm $callgrindFile  # I guess?  Maybe give option to save in a separate directory
