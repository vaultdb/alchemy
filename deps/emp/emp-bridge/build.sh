#!/bin/bash

EMP_BRIDGE_PATH=$1
EMP_FILE=$2

SRC_FILE=$EMP_BRIDGE_PATH'/src/'$EMP_FILE'.cpp'


/usr/bin/c++   -DEMP_CIRCUIT_PATH=/usr/local/include/emp-tool/circuits/files/ -DEMP_USE_RANDOM_DEVICE -I$EMP_BRIDGE_PATH/include -I/usr/local/include/include -I/usr/local/include -I/usr/include/x86_64-linux-gnu  -pthread -Wall -march=native -O3 -maes -mrdseed -std=c++11 -O3 -DNDEBUG   -o $EMP_BRIDGE_PATH/bin/$EMP_FILE -c $SRC_FILE
