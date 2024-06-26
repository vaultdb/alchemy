#!/bin/bash

killall -q -9 $1 #clean up any previous instances
sleep 0.05
make -j  $1

if [[ $? -ne 0 ]]; then #abort if build fails
    exit 1
fi


[ -d log ] || mkdir log

./bin/$1  --flagfile=flagfiles/tp-ompc.flags  | tee  'log/'$1'-p10086.log' &
sleep 0.05
./bin/$1 --flagfile=flagfiles/alice-ompc.flags > 'log/'$1'-p1.log'  &
 sleep 0.05
 ./bin/$1  --flagfile=flagfiles/bob-ompc.flags > 'log/'$1'-p2.log'    &
 sleep 0.05
./bin/$1   --flagfile=flagfiles/chi-ompc.flags > 'log/'$1'-p3.log'
