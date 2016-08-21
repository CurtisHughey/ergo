#!/bin/bash

# Add debugging flag ^^^

dim=19  # The default dimension
valgrind="VALGRIND="  # Nothing by default
optlevel="OPTLEVEL=-O3"  # By default, the highest

while [[ $# -gt 0 ]]
do
	key="$1"

	case $key in
	    -d)
		    dim="$2"
			case $dim in
			    *[0123456789]) ;;  # Silently pass (this isn't really correct, since it allows nothing, but it is covered from above)
			    *) echo "ERROR: Board dimension must be an integer, got $dim, exiting"; exit ;;
			esac

			if [[ $dim -lt 1 ]]
			then
				echo "ERROR: Board dimension must be greater than 0, got $dim, exiting"
				exit
			fi

			if [[ $dim -gt 19 ]]
			then
				echo "ERROR: Board dimension must be less than 20, got $dim, exiting"
				exit
			fi

		    shift # past argument
		    ;;

	    -v)
			valgrind="VALGRIND=-g"
		    ;;
		-n)   # No optimization (used in profiling)
			optlevel="OPTLEVEL="
			;;
	    *)
	    	echo "ERROR: Unknown command line option: $1, exiting"
	    	exit
	   		;;
	esac
	shift # past argument or value
done

cd src  # Moves to the actual source directory to make

echo "CLEANING"
make clean
echo ""
echo "COMPILING"
echo "Board dimension: $dim"
make DIM=-DBOARD_DIM=$dim $valgrind $optlevel
if [ $? -eq 0 ]
then 
	echo ""
	echo "FINISHED"
	cp ergo ../ergo  # Moves it back to the root directory
	make clean  # Don't need those files anymore
	cd ..  # And returns there as well
	exit 0
else
	echo ""
	echo "COMPILATION FAILED"
	cd ..  # Still returns
	exit 1
fi
