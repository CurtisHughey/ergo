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

echo "5x5 Serial Wins"
./build.sh -d 5 &>/dev/null  # Bigger for more variation

# Now need to give custom configurations
echo "komiTimes10 0" >> $tempConfig
echo "rollouts 500" >> $tempConfig
echo "superko 1" >> $tempConfig 
echo "threads 1" >> $tempConfig

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

echo "5x5 Parallel Wins"
./build.sh -d 5 &>/dev/null  # Parallel

echo "komiTimes10 0" >> $tempConfig
echo "rollouts 1000" >> $tempConfig
echo "superko 1" >> $tempConfig
echo "threads 2" >> $tempConfig  # Multi threads

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

echo "5x5 Serial RAVE Wins"
./build.sh -d 5 &>/dev/null  # Bigger for more variation

# Now need to give custom configurations
echo "komiTimes10 0" >> $tempConfig
echo "rollouts 500" >> $tempConfig
echo "superko 1" >> $tempConfig
echo "threads 1" >> $tempConfig
echo "raveV 5" >> $tempConfig  # Now we set the rave constant for AMAF/RAVE

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

echo "5x5 Parallel RAVE Wins"
./build.sh -d 5 &>/dev/null  # Bigger for more variation

# Now need to give custom configurations
echo "komiTimes10 0" >> $tempConfig
echo "rollouts 500" >> $tempConfig
echo "superko 1" >> $tempConfig 
echo "threads 2" >> $tempConfig
echo "raveV 5" >> $tempConfig  # Now we set the rave constant for AMAF/RAVE

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

valgrind -v --num-callers=100 --trace-children=yes --error-exitcode=1 --leak-check=full --track-origins=yes --show-leak-kinds=all ./ergo -u -C $tempConfig &>> $testLog  # This is probably overkill
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

echo "MCTS Serial Simulation"  # Computer vs computer, serial
./build.sh -d 3 -v &>/dev/null

echo "komiTimes10 0" >> $tempConfig
echo "rollouts 100" >> $tempConfig  # Doesn't need to be fast
echo "threads 1" >> $tempConfig
echo "superko 1" >> $tempConfig

valgrind -v --num-callers=100 --trace-children=yes --error-exitcode=1 --leak-check=full --track-origins=yes --show-leak-kinds=all ./ergo -x -C $tempConfig &>> $testLog # This is probably overkill
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

echo "MCTS Parallel Simulation"  # Computer vs computer
./build.sh -d 3 -v &>/dev/null

echo "komiTimes10 0" >> $tempConfig
echo "rollouts 2" >> $tempConfig  # Very low, otherwise Valgrind takes forever with the thread stuff, I guess (I think because Valgrind runs only on 1 core)
echo "threads 2" >> $tempConfig
echo "superko 1" >> $tempConfig

valgrind -v --num-callers=100 --trace-children=yes --error-exitcode=1 --leak-check=full --track-origins=yes --show-leak-kinds=all ./ergo -x -C $tempConfig &>> $testLog # This is probably overkill
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

echo "MCTS Serial RAVE Simulation"  # Computer vs computer
./build.sh -d 3 -v &>/dev/null

echo "komiTimes10 0" >> $tempConfig
echo "rollouts 100" >> $tempConfig  # Very low, otherwise Valgrind takes forever with the thread stuff, I guess (I think because Valgrind runs only on 1 core)
echo "threads 1" >> $tempConfig
echo "superko 1" >> $tempConfig
echo "raveV 5" >> $tempConfig

valgrind -v --num-callers=100 --trace-children=yes --error-exitcode=1 --leak-check=full --track-origins=yes --show-leak-kinds=all ./ergo -x -C $tempConfig &>> $testLog # This is probably overkill
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

echo "MCTS Parallel RAVE Simulation"  # Computer vs computer
./build.sh -d 3 -v &>/dev/null

echo "komiTimes10 0" >> $tempConfig
echo "rollouts 2" >> $tempConfig  # Very low, otherwise Valgrind takes forever with the thread stuff, I guess (I think because Valgrind runs only on 1 core)
echo "threads 2" >> $tempConfig
echo "superko 1" >> $tempConfig
echo "raveV 5" >> $tempConfig

valgrind -v --num-callers=100 --trace-children=yes --error-exitcode=1 --leak-check=full --track-origins=yes --show-leak-kinds=all ./ergo -x -C $tempConfig &>> $testLog # This is probably overkill
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
