#!/bin/bash -x

ALICE_LOG=$1
BOB_LOG=$2

make -j5

rm $ALICE_LOG $BOB_LOG


./bin/secure_filter_test --party=1 2>&1 >> $ALICE_LOG &
./bin/secure_filter_test --party=2 2>&1 >> $BOB_LOG

sleep 2
./bin/secure_sort_test --party=1 2>&1 >> $ALICE_LOG &
./bin/secure_sort_test --party=2 2>&1 >> $BOB_LOG

sleep 2
./bin/secure_basic_join_test --party=1 2>&1 >> $ALICE_LOG &
./bin/secure_basic_join_test --party=2 2>&1 >> $BOB_LOG

sleep 2
./bin/secure_pkey_fkey_join_test --party=1 2>&1 >> $ALICE_LOG &
./bin/secure_pkey_fkey_join_test --party=2 2>&1 >> $BOB_LOG

sleep 2
./bin/secure_scalar_aggregate_test --party=1 2>&1 >> $ALICE_LOG &
./bin/secure_scalar_aggregate_test --party=2 2>&1 >> $BOB_LOG

sleep 2
./bin/secure_group_by_aggregate_test --party=1 2>&1 >> $ALICE_LOG &
./bin/secure_group_by_aggregate_test --party=2 2>&1 >> $BOB_LOG




#process with:
#for((RUN=1; RUN<4; ++RUN)); do LOG='log/mpc/'$RUN'-bob.log'; grep 'ms)' $LOG | sed 's/^.*Secure/Secure/g' | sed 's/\ ms.//g' | sed 's/\ ./,/g' | sed 's/Test//g' | sed 's/test_//g' | sed 's/Secure//g' > log/mpc/$RUN.csv; done

