#!/bin/bash -x

#usage: ./run-bob.sh <testname>
#e.g., ./run-bob.sh secure_sort_test

if [ $# -lt 2 ];
then
   printf "usage:  ./run-bob.sh <testname> <alice_host>\n"
   printf "Not enough arguments - %d\n" $# 
   exit 0 
fi

./bin/$1 --flagfile=flagfiles/bob.flags --alice_host=$2
