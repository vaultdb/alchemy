#!/bin/bash

if [ $# -lt 1 ];
then
   printf "usage:  ./benchmark-suite-bob.sh  <alice_host>\n"
   printf "Not enough arguments - %d\n" $# 
   exit 0 
fi

now=`date +"%Y%m%d%H%M"`
file='log/all-tests-'$now'.log'

cmake -DCMAKE_BUILD_TYPE=Release .
make clean

echo 'Writing to '$file

bash run-tests-bob.sh $1 2>&1 > $file

#print results
grep '^\[' $file

# to record test in git
# git add $(ls log/all-tests-*.log | tail -n 1)
# grep SecureTpcHTest $file  | grep 'ms)' | uniq | sed 's/^.*(//g' | sed 's/\ .*$//g'
