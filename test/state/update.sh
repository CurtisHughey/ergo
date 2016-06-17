#! /bin/bash
# Small helper file to update serialized states if parser.c/state.h changes
for f in */[ie]*.txt
do
	echo -e "\n0" >> "$f"   # this isn't good if there are comments at end ^^^
done