#!/bin/bash

DIM=19  # The default dimension

if [ -n "$1" ]
then
	DIM=$1
fi

case $DIM in
    *[0123456789]) ;;  # Silently pass (this isn't really correct, since it allows nothing, but it is covered from above)
    *) echo "Board dimension must be an integer, got $DIM"; exit ;;
esac

if [ $DIM -lt 1 ]
then
	echo "Board dimension must be greater than 0, got $DIM"
	exit
fi

if [ $DIM -gt 19 ]
then
	echo "Board dimension must be less than 20, got $DIM"
	exit
fi

echo "CLEANING"
make clean
echo ""
echo "COMPILING"
echo "Board dimension: $DIM"
make DIM=-DBOARD_DIM=$DIM
echo ""
echo "FINISHED"