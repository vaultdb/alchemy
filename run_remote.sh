#!/bin/bash
if [ "$#" -ne 2 ]; then
  printf "Usage: ./run_remote.sh <arg1> <arg2> 
    arg1: hostname for worker1
    arg2: hostname for worker2\n"
  exit 1
fi
name=/tmp/alchemy_$(date +%Y%m%d_%H%M%S)

executionString2='ssh $1 "git clone git@github.com:jennierogers/alchemy.git $name && cd $name && git checkout alchemy &&  mvn -Dtest=org.smcql.executor.remote.RPCExecutorServiceTest#testStartServer test"'
executionString1='ssh $2 "git clone git@github.com:jennierogers/alchemy.git $name && cd $name && git checkout alchemy &&  mvn -Dtest=org.smcql.executor.remote.RPCExecutorServiceTest#testStartServer test"'
eval $executionString1&
eval $executionString2&
wait

