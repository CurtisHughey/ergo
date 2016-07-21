#!/bin/bash

# Adds necessary directories and compiles default version
# UNTESTED.  Will also want to add kgs stuff ^^^

kgsGtpDir="kgsGtp"

mkdir configs
mkdir output
mkdir log
mkdir "$kgsGtpDir/log" 

chmod u+x build.sh  # Might not need
./build.sh  # Default
