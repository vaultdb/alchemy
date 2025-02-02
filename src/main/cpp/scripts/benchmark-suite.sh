#!/bin/bash

now=`date +"%Y%m%d%H%M"`
file='log/all-tests-localhost-'$now'.log'


echo 'Writing to '$file

bash scripts/run-tests.sh 2>&1 > $file

#print results
#grep '^\[' $file
grep FAIL $file

# to record test in git
#file=$( ls -t log/all-tests-2*.log | head -n 1)
# git add $(ls -t log/all-tests-2*.log | head -n 1)
# grep SecureTpcHTest $file  | grep 'ms)' | uniq | sed 's/^.*(//g' | sed 's/\ .*$//g'

# grep OK  $file  | sed 's/^.*OK...//g'  | sed 's/^.*(//g' | sed 's/\ .*$//g'
