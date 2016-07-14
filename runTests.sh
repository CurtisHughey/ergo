#!/bin/bash

tempConfig="configs/tempConfig.txt"
rm -f $tempConfig  # In case it's already there

numPassed=0
numTests=0

# First unit tests
echo "--------------------"
echo "Unit Tests"
./build.sh -d 19 &>/dev/null

./ergo -u &>/dev/null
if [[ $? -eq 0 ]]
then
	echo "Passed :)"
	numPassed=$((numPassed+1))
else
	echo "Failed :("
fi
numTests=$((numTests+1))
echo "--------------------"
#########################################

# Memory tests
echo "--------------------"
echo "Memory Tests"
echo "Unit"
./build.sh -v &>/dev/null   # Need to track for valgrind, needs to be default for unit

# # Now need to give custom configurations
# echo "rollouts 5" >> $tempConfig  # Doesn't need to be fast

echo "unitRandomMakeUnmakeTests 1" >> $tempConfig  # We'll test this stuff more later

valgrind --error-exitcode=1 --leak-check=full --track-origins=yes --show-leak-kinds=all -v ./ergo -u -C $tempConfig &>/dev/null  # This is probably overkill
if ! [[ $? -eq 1 ]]
then
	echo "Passed :)"
	numPassed=$((numPassed+1))
else
	echo "Failed :("
fi
numTests=$((numTests+1))
rm $tempConfig

echo "Simulation"  # Computer vs computer
./build.sh -d 3 -v &>/dev/null

echo "rollouts 100000" >> $tempConfig  # Doesn't need to be fast

valgrind --error-exitcode=1 --leak-check=full --track-origins=yes --show-leak-kinds=all -v ./ergo -x &>/dev/null # This is probably overkill
if ! [[ $? -eq 1 ]]
then
	echo "Passed :)"
	numPassed=$((numPassed+1))
else
	echo "Failed :("
fi
numTests=$((numTests+1))
rm $tempConfig
echo "--------------------"
#########################################


# Correct AI tests (ish)
echo "--------------------"
echo "MCTS Correctness Tests"
./build.sh -d 3 &>/dev/null  # 19 is way too big

# Now need to give custom configurations
echo "rollouts 1000" >> $tempConfig

./ergo -y -C $tempConfig &>/dev/null
if [[ $? -eq 1 ]]
then
	echo "Passed :)"
	numPassed=$((numPassed+1))
else
	echo "Failed :("
fi
numTests=$((numTests+1))
rm $tempConfig
echo "--------------------"
#########################################


./build.sh &> /dev/null  # Restores it to the default

echo "$numPassed/$numTests passed"
if [[ $numPassed -eq $numTests ]]
then
	echo "All Passed :)"
else
	echo "Failed :("
fi
echo "--------------------"
