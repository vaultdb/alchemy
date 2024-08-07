#!/bin/bash -x

if [ "$#" -ne 1 ]; then
  #example: bash scripts/generate-flamegraph.sh bin/ompc_block_nested_loop_join_test
    echo "usage: scripts/generate-flamegraph.sh <test file>"
    exit
fi


PERF_EXE=`which perf`
TEST=$1
#if missing, pull with:
# cd _deps && git clone https://github.com/brendangregg/FlameGraph
FLAMEGRAPH="_deps/FlameGraph"

$PERF_EXE record -F 100 -a --call-graph dwarf -m256M $TEST --party=1 & $TEST --party=2 & $TEST --party=3 & $TEST --party=10086
wait
sleep 1
sync
$PERF_EXE script -f | ${FLAMEGRAPH}/stackcollapse-perf.pl > out.folded && ${FLAMEGRAPH}/flamegraph.pl out.folded > `basename ${TEST}`.svg wait
########################### # try pprof here ########################### $exe --party=2 &
env CPUPROFILE=`basename ${TEST}.prof` $TEST --party=1
wait
/usr/local/bin/pprof --pdf $TEST `basename ${TEST}.prof` &> `basename ${TEST}.pdf` echo "script done"