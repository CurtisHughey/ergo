#!/bin/bash

numRequired=$((361*3+1))  # At most, we need white, black, blank for each square on a 19x19 board, plus the ko move

OUTPUTFILE="randomNumbers.txt"
> $OUTPUTFILE  # Overwriting

for ((i=1;i<=numRequired;i++))
do
	if [ $(($i%50)) -eq 0 ]  # Just to give updates
	then
		echo "Processing: $i"
	fi		
	randomNum=$(od -An -N8 -x /dev/random | tr -d ' ')  # -N8 to get 64 bit numbers
	echo -e $randomNum >> $OUTPUTFILE
done

echo "Finished!"