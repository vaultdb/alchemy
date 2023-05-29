#!/bin/bash

now=`date +"%Y%m%d%H%M"`
file='log/all-tests-alice-'$now'.log'

cmake -DCMAKE_BUILD_TYPE=Release .
make clean

echo 'Writing to '$file

bash run-tests-alice.sh 2>&1 > $file

#print results
grep '^\[' $file
grep 'FAIL' $file

# to record test in git
# git add $(ls log/all-tests-*.log | tail -n 1)
# file=$(ls -t log/*alice* | head -n 1)
# grep '^\[' $file | grep 'ms)' |  sed 's/^.*(//g' | sed 's/\ .*$//g' | pbcopy