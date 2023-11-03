#!/bin/bash

# default alice host: codd10


#if [ $# -lt 1 ];
#then
#   printf "usage:  ./benchmark-suite-bob.sh  <alice_host>\n"
#   printf "Not enough arguments - %d\n" $# 
#   exit 0 
#fi

if [ $# -lt 1 ];
then
    
   ALICE_HOST="129.105.61.184"
else
    ALICE_HOST=$1
fi


       
now=`date +"%Y%m%d%H%M"`
FILE='log/all-tests-bob-column-store-'$now'.log'

cmake -DCMAKE_BUILD_TYPE=Release .
make all

echo 'Writing to '$FILE

bash scripts/run-tests-bob.sh $ALICE_HOST 2>&1 > $FILE

#print results
grep '^\[' $FILE
grep 'FAIL' $FILE


# to record test in git
# git add $(ls log/all-tests-bob*.log | tail -n 1)
# FILE=$(ls -t log/*bob* | head -n 1)
# grep 'ms)'  $FILE | egrep 'Secure|Emp' |  sed 's/^.*(//g' | sed 's/\ .*$//g' | pbcopy
