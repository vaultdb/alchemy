#!/bin/bash -x

#usage: ./run-alice.sh <testname>
#e.g., ./run-alice.sh secure_sort_test

if [ $# -lt 1 ]; 
then
   printf "usage:  ./run-alice.sh <testname>\n"
   printf "Not enough arguments - %d\n" $# 
   exit 0 
fi

./bin/$1 --flagfile=flagfiles/alice.flags 
