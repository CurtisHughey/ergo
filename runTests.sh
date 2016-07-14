#!/bin/bash

numPassed=0
numTests=0

# First unit tests
echo "--------------------"
echo "Unit Tests"
./build.sh 19 &>/dev/null
./ergo -u &>/dev/null
if [[ $? -eq 1 ]]
then
	echo "Passed :)"
	numPassed=$((numPassed+1))
else
	echo "Failed :("
fi
numTests=$((numTests+1))
echo "--------------------"


# # Correct AI tests (ish)
# echo "--------------------"
# echo "MCTS Correctness Tests"
# ./build.sh 19 &>/dev/null
# ./ergo -u &>/dev/null
# if [[ $? -eq 1 ]]
# then
# 	echo "Passed :)"
# 	numPassed=$((numPassed+1))
# else
# 	echo "Failed :("
# fi
# numTests=$((numTests+1))
# echo "--------------------"


./build.sh 19 &> /dev/null

echo "$numPassed/$numTests passed"
if [[ $numPassed -eq $numTests ]]
then
	echo "All Passed :)"
else
	echo "Failed :("
fi
echo "--------------------"
