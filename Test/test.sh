#!/bin/bash

make clean
numShell=$1
TestFile=$2

if [ $numShell -eq 1 ]; then
  make TEST=1
  ShellCur="./Bin/shell1"
elif [ $numShell -eq 2 ]; then
  make MAIN2=1 TEST=1
  ShellCur="./Bin/shell2"
elif [ $numShell -eq 3 ]; then
  make MAIN3=1 TEST=1
  ShellCur="./Bin/shell3"
fi

echo "Running test ${TestFile} with shell V${numShell}"

./sdriver.pl -t ${TestFile} -s ${ShellCur}>tmp.txt