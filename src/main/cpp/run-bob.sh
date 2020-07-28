#!/bin/bash -x

#usage: ./run-bob.sh <testname>
#e.g., ./run-bob.sh secure_sort_test

if [ $# -lt 1 ]; 
then
   printf "usage:  ./run-bob.sh <testname>\n"
   printf "Not enough arguments - %d\n" $# 
   exit 0 
fi

./bin/$1 --party=2
