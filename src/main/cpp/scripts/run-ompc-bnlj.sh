#!/bin/bash

if [ "$#" -ne 2 ]; then
    echo "usage: scripts/run-ompc-bnlj.sh <storage model> <input size>"
    # example: scripts/run-ompc-bnlj.sh wire_packed m
    exit
fi

MEM_LIMIT=500000000 # 250 MB
STORAGE=$1 # #column or wire_packed
INPUT_SIZE=$2 # s, m or l


killall -q -9 ompc_block_nested_loop_join_test  #clean up any previous instances
sleep 0.05
make -j6 ompc_block_nested_loop_join_test

if [[ $? -ne 0 ]]; then #abort if build fails
    exit 1
fi


[ -d log ] || mkdir log

(ulimit -m $MEM_LIMIT && ./bin/ompc_block_nested_loop_join_test  --flagfile=flagfiles/tp-ompc.flags --storage=$STORAGE --input_size=$INPUT_SIZE | tee  log/ompc_block_nested_loop_join_test-p10086.log) &
sleep 0.05

(ulimit -m $MEM_LIMIT && ./bin/ompc_block_nested_loop_join_test --flagfile=flagfiles/alice-ompc.flags --storage=$STORAGE --input_size=$INPUT_SIZE > log/ompc_block_nested_loop_join_test-p1.log)  &
 sleep 0.05
(ulimit -m $MEM_LIMIT && ./bin/ompc_block_nested_loop_join_test  --flagfile=flagfiles/bob-ompc.flags --storage=$STORAGE --input_size=$INPUT_SIZE > log/ompc_block_nested_loop_join_test-p2.log)    &

sleep 0.05

(ulimit -m $MEM_LIMIT && ./bin/ompc_block_nested_loop_join_test   --flagfile=flagfiles/chi-ompc.flags --storage=$STORAGE --input_size=$INPUT_SIZE > log/ompc_block_nested_loop_join_test-p3.log)
