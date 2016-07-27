#!/bin/bash

# This runs unit, memory, and AI correctness tests
# DO NOT modify any files while this is running

tempConfig="configs/tempConfig.txt"
testLog="log/test.log"

rm -f $tempConfig  # In case it's already there
rm -f $testLog

echo "Started at: " >> $testLog
date >> $testLog

numPassed=0
numTests=0

# First verify that it correctly compiles
./build.sh &>/dev/null
if [[ $? -ne 0 ]]
then
	echo "Failed to compile"
	exit 1
fi

# First unit tests
echo "--------------------"
echo "Unit Tests"
./build.sh -d 19 &>/dev/null

echo "komiTimes10 75" >> $tempConfig   # Needs to make sure the komi is correct when calculating scores

./ergo -u -C $tempConfig &>> $testLog
if [[ $? -eq 0 ]]
then
	echo "Passed :)"
	numPassed=$((numPassed+1))
else
	echo "Failed :("
fi
numTests=$((numTests+1))
rm $tempConfig
echo "--------------------"

echo "--------------------------------------------------------------------------------" >> $testLog
#########################################


# Correct AI tests (ish)
echo "--------------------"
echo "MCTS Correctness Tests"
echo "3x3 Wins"
./build.sh -d 3 &>/dev/null  # 19 is way too big

# Now need to give custom configurations
echo "komiTimes10 0" >> $tempConfig
echo "rollouts 1000" >> $tempConfig

./ergo -y -C $tempConfig &>> $testLog
if [[ $? -eq 1 ]]
then
	echo "Passed :)"
	numPassed=$((numPassed+1))
else
	echo "Failed :("
fi
numTests=$((numTests+1))
rm $tempConfig
echo "---"

echo "3x3 Loses"
./build.sh -d 3 &>/dev/null

echo "komiTimes10 0" >> $tempConfig
echo "rollouts 1" >> $tempConfig  # Sanity check to make sure we suck if the number of rollouts is low

./ergo -y -C $tempConfig &>> $testLog
if [[ $? -eq 0 ]]  # Wants to fail now
then
	echo "Passed :)"
	numPassed=$((numPassed+1))
else
	echo "Failed :("
fi
numTests=$((numTests+1))
rm $tempConfig
echo "---"

./build.sh -d 6 &>/dev/null  # Bigger for more variation

# Now need to give custom configurations
echo "komiTimes10 0" >> $tempConfig
echo "rollouts 500" >> $tempConfig

echo "6x6"
./ergo -y -C $tempConfig &>> $testLog
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

echo "--------------------------------------------------------------------------------" >> $testLog
#########################################


# Memory tests
echo "--------------------"
echo "Memory Tests"
echo "Unit"
./build.sh -v &>/dev/null   # Need to track for valgrind, needs to be default for unit

# # Now need to give custom configurations
# echo "rollouts 5" >> $tempConfig  # Doesn't need to be fast

echo "komiTimes10 75" >> $tempConfig  

valgrind --error-exitcode=1 --leak-check=full --track-origins=yes --show-leak-kinds=all ./ergo -u -C $tempConfig &>> $testLog  # This is probably overkill
if ! [[ $? -eq 1 ]]
then
	echo "Passed :)"
	numPassed=$((numPassed+1))
else
	echo "Failed :("
fi
numTests=$((numTests+1))
rm $tempConfig
echo "---"

echo "MCTS Simulation"  # Computer vs computer
./build.sh -d 3 -v &>/dev/null

echo "komiTimes10 0" >> $tempConfig
echo "rollouts 100" >> $tempConfig  # Doesn't need to be fast

valgrind --error-exitcode=1 --leak-check=full --track-origins=yes --show-leak-kinds=all ./ergo -x -C $tempConfig &>> $testLog # This is probably overkill
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

echo "--------------------------------------------------------------------------------" >> $testLog
#########################################


./build.sh &> /dev/null  # Restores it to the default

echo "$numPassed/$numTests passed"
if [[ $numPassed -eq $numTests ]]
then
	echo "All Passed :)"
else
	echo "Failed :("
	echo "Check $testLog for details"
fi
echo "--------------------"

echo "Finished at: " >> $testLog
date >> $testLog
