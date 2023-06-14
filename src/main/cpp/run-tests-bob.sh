#!/bin/bash -x

if [ $# -lt 1 ];
then
   printf "usage:  bash /run-tests-bob.sh  <alice_host>\n"
   printf "Not enough arguments - %d\n" $# 
   exit 0 
fi


cmake -DCMAKE_BUILD_TYPE=Release .
make -j

bash test/support/setup-csv.sh
bash pilot/test/generate-and-load-test-data.sh 1000
# regenerate test data
bash  test/support/load-generated-data.sh 100


./bin/csv_reader_test
./bin/field_expression_test
./bin/filter_test
./bin/project_test
./bin/basic_join_test
./bin/keyed_join_test
./bin/sort_test
./bin/scalar_aggregate_test
./bin/group_by_aggregate_test
./bin/secret_share_generator_test
#this test depends on pilot testbed
./bin/serialization_test
./bin/plan_parser_test
./bin/tpch_test

ALICE_HOST=$1
./bin/emp_test --party=2 --alice_host=$ALICE_HOST
./bin/secure_field_expression_test  --party=2 --alice_host=$ALICE_HOST
./bin/emp_table_test  --party=2 --alice_host=$ALICE_HOST
./bin/secure_filter_test  --party=2 --alice_host=$ALICE_HOST
./bin/secure_sort_test  --party=2 --alice_host=$ALICE_HOST
./bin/secure_basic_join_test  --party=2 --alice_host=$ALICE_HOST
./bin/secure_keyed_join_test  --party=2 --alice_host=$ALICE_HOST
./bin/secure_scalar_aggregate_test   --party=2 --alice_host=$ALICE_HOST
./bin/secure_group_by_aggregate_test  --party=2 --alice_host=$ALICE_HOST
#./bin/enrich_test  --party=2 --alice_host=$ALICE_HOST # excluded because data is not synced between A + B
./bin/secure_tpch_test  --party=2 --alice_host=$ALICE_HOST
