#!/bin/bash

make -j  $1

if [[ $? -ne 0 ]]; then #abort if build fails
    exit 1
fi

killall -q $1 #clean up any previous instances
#[ -d log ] || mkdir log

./$1 --party=10086   > 'log/'$1'-p10086.log' &
sleep 0.05
./$1 --party=1 > 'log/'$1'-p1.log'  &
 sleep 0.05
 ./$1 --party=2  > 'log/'$1'-p2.log'    &
 sleep 0.05
./$1 --party=3 > 'log/'$1'-p3.log'
