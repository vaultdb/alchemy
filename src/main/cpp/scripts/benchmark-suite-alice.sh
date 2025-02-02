#!/bin/bash

now=`date +"%Y%m%d%H%M"`
file='log/all-tests-alice-column-store-'$now'.log'

cmake -DCMAKE_BUILD_TYPE=Release .
make all -j6

echo 'Writing to '$file

bash scripts/run-tests-alice.sh 2>&1 > $file

#print results
grep '^\[' $file
grep 'FAIL' $file

# to record test in git
# git add $(ls log/all-tests-alice*.log | tail -n 1)
# FILE=$(ls -t log/*alice* | head -n 1)
# grep '^\[' $FILE | grep 'ms)' |  sed 's/^.*(//g' | sed 's/\ .*$//g' | pbcopy
