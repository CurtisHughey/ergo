#!/bin/bash

# Adds necessary directories and compiles default version
# UNTESTED.  Will also want to add kgs stuff ^^^

kgsGtpDir="kgsGtp"
regTestsDir="regTests"

mkdir "output"
mkdir "log"
mkdir "configs"
mkdir "$kgsGtpDir/log" 
mkdir "$regTests/log"
mkdir "$regTests/results"

chmod u+x build.sh  # Might not need
./build.sh  # Default
