#!/bin/bash
TEST=$1
echo "Running $TEST"
killall -q -9 $TEST #clean up any previous instances
sleep 0.05
make -j4  $TEST

if [[ $? -ne 0 ]]; then #abort if build fails
    exit 1
fi


[ -d log ] || mkdir log

./bin/$TEST  --flagfile=flagfiles/tp-ompc.flags  | tee  'log/'$TEST'-p10086.log' &
sleep 0.05
./bin/$TEST --flagfile=flagfiles/alice-ompc.flags > 'log/'$TEST'-p1.log'  &
 sleep 0.05
 ./bin/$TEST  --flagfile=flagfiles/bob-ompc.flags > 'log/'$TEST'-p2.log'    &
 sleep 0.05
./bin/$TEST   --flagfile=flagfiles/chi-ompc.flags > 'log/'$TEST'-p3.log'
