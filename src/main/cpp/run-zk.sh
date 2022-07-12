#!/bin/bash -x

ALICE_LOG=$1
BOB_LOG=$2

make -j5

rm $ALICE_LOG $BOB_LOG

./bin/zk_test --party=1 2>&1 >> $ALICE_LOG &
./bin/zk_test  --party=2 2>&1 >> $BOB_LOG

sleep 2
./bin/zk_filter_test --party=1 2>&1 >> $ALICE_LOG &
./bin/zk_filter_test --party=2 2>&1 >> $BOB_LOG

sleep 2
./bin/zk_sort_test --party=1 2>&1 >> $ALICE_LOG &
./bin/zk_sort_test --party=2 2>&1 >> $BOB_LOG

sleep 2
./bin/zk_basic_join_test --party=1 2>&1 >> $ALICE_LOG &
./bin/zk_basic_join_test --party=2 2>&1 >> $BOB_LOG

sleep 2
./bin/zk_keyed_join_test --party=1 2>&1 >> $ALICE_LOG &
./bin/zk_keyed_join_test --party=2 2>&1 >> $BOB_LOG

sleep 2
./bin/zk_scalar_aggregate_test --party=1 2>&1 >> $ALICE_LOG &
./bin/zk_scalar_aggregate_test --party=2 2>&1 >> $BOB_LOG

sleep 2
./bin/zk_group_by_aggregate_test --party=1 2>&1 >> $ALICE_LOG &
./bin/zk_group_by_aggregate_test --party=2 2>&1 >> $BOB_LOG

sleep 2

./bin/zk_tpch_test --party=1  2>&1 >> $ALICE_LOG &
./bin/zk_tpch_test --party=2 2>&1 >> $BOB_LOG
