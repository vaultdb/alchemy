#!/bin/bash -x

if [ $# -lt 1 ];
then
   printf "usage:  ./run-tests-bob.sh  <alice_host>\n"
   printf "Not enough arguments - %d\n" $# 
   exit 0 
fi


cmake -DCMAKE_BUILD_TYPE=Release .
make -j

bash test/support/setup-csv.sh
bash pilot/test/generate-and-load-test-data.sh 1000
# regenerate test data
bash  test/support/load-generated-data.sh 100

# optional:
#  bash run-pilot-test.sh


./csv_reader_test 
./field_expression_test
./filter_test
./project_test
./basic_join_test
./keyed_join_test
./sort_test
./scalar_aggregate_test
./group_by_aggregate_test
./secret_share_generator_test 
#this test depends on pilot testbed
./serialization_test
./plan_parser_test
./tpch_test

ALICE_HOST=$1
./emp_test --party=2 --alice_host=$ALICE_HOST
./secure_field_expression_test  --party=2 --alice_host=$ALICE_HOST
./emp_table_test  --party=2 --alice_host=$ALICE_HOST
./secure_filter_test  --party=2 --alice_host=$ALICE_HOST
./secure_sort_test  --party=2 --alice_host=$ALICE_HOST
./secure_basic_join_test  --party=2 --alice_host=$ALICE_HOST
./secure_keyed_join_test  --party=2 --alice_host=$ALICE_HOST
./secure_scalar_aggregate_test   --party=2 --alice_host=$ALICE_HOST
./secure_group_by_aggregate_test  --party=2 --alice_host=$ALICE_HOST
#./enrich_test  --party=2 --alice_host=$ALICE_HOST test not synced on input data
./secure_tpch_test  --party=2 --alice_host=$ALICE_HOST
