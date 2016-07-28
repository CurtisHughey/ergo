#!/bin/bash

kgsGtpDir="kgsGtp"

dim=19  # Defaults to 19x19 board
if [[ -n "$1" ]]
then
	dim="$1"
else 
	echo "Defaulting to $dim length board"
fi

./build.sh -d "$dim"

if [[ "$?" -ne 0 ]]  # Checks for compilation failure
then
	echo "Compilation failed, can't connect to KGS server"
	exit 1
fi

cp ergo "$kgsGtpDir/ergo"  # Copies executable to subdirectory
cp -r zobristData "$kgsGtpDir/"

# Otherwise, good to go!

cd "$kgsGtpDir"

gamesPlayed=0
while true
do
	java -jar kgsGtp.jar kgsConfigs/defaultKgsConfig.txt "rules.boardSize=$dim"  # dim overrides what's provided in the default file
	
	if [[ $? -gt 128 ]]  # I.e. if error, ctl-c, etc., we quit
	then
		break
	else  # Successfully played a game
		gamesPlayed=$((gamesPlayed+1))  # Eventually do more to check if won, lost, etc. ^^^
		echo "Games played: $gamesPlayed"
	fi
done

cd ..