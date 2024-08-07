#!/bin/bash

MEM_LIMIT=500000000 # 500 MB
STORAGE=column #column or wire_packed


killall -q -9 $1 #clean up any previous instances
sleep 0.05
make -j  $1

if [[ $? -ne 0 ]]; then #abort if build fails
    exit 1
fi


[ -d log ] || mkdir log

(ulimit -m $MEM_LIMIT && ./bin/$1  --flagfile=flagfiles/tp-ompc.flags --storage=$STORAGE | tee  'log/'$1'-p10086.log') &
sleep 0.05

(ulimit -m $MEM_LIMIT && ./bin/$1 --flagfile=flagfiles/alice-ompc.flags --storage=$STORAGE  > 'log/'$1'-p1.log')  &
 sleep 0.05
(ulimit -m $MEM_LIMIT && ./bin/$1  --flagfile=flagfiles/bob-ompc.flags --storage=$STORAGE  > 'log/'$1'-p2.log')    &
 sleep 0.05
(ulimit -m $MEM_LIMIT && ./bin/$1   --flagfile=flagfiles/chi-ompc.flags --storage=$STORAGE  > 'log/'$1'-p3.log')
