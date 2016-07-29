#!/bin/bash

regTestsDir="regTests"

./build.sh -d 19  # Always builds 19x19

if [[ "$?" -ne 0 ]]  # Checks for compilation failure
then
	echo "Compilation failed, can't connect to KGS server"
	exit 1
fi

cp ergo "$regTestsDir/ergo"  # Copies executable to subdirectory
cp -r zobristData "$regTestsDir/"

# Otherwise, good to go!

cd "$regTestsDir"

java -jar gogui/lib/gogui-regress.jar ./runTranslator.sh cgtc/*.tst  # Can include verbose

# Moves results
for htmlFile in *.html
do
	mv "$htmlFile" "results/$htmlFile"
done

mv "summary.dat" "results/$summary.dat"

cd ..