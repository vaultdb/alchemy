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

# optional:
#  bash run-pilot-test.sh

./bin/csv_reader_test --storage=column
./bin/field_expression_test --storage=column
./bin/filter_test  --storage=column
./bin/project_test  --storage=column
./bin/basic_join_test --storage=column
./bin/keyed_join_test --storage=column
./bin/sort_merge_join_test --storage=column
./bin/sort_test --storage=column
./bin/scalar_aggregate_test --storage=column
./bin/group_by_aggregate_test --storage=column
./bin/nested_loop_aggregate_test --storage=column
./bin/secret_share_generator_test --storage=column
#this test depends on pilot testbed
./bin/serialization_test --storage=column
./bin/plan_parser_test --storage=column
./bin/tpch_test --storage=column


ALICE_HOST=$1
./bin/emp_test --party=2 --storage=column --alice_host=$ALICE_HOST
./bin/secure_field_expression_test  --party=2 --storage=column --alice_host=$ALICE_HOST
./bin/emp_table_test  --party=2 --storage=column --alice_host=$ALICE_HOST
./bin/secure_filter_test  --party=2 --storage=column --alice_host=$ALICE_HOST
./bin/secure_sort_test  --party=2 --storage=column --alice_host=$ALICE_HOST
./bin/secure_basic_join_test  --party=2 --storage=column --alice_host=$ALICE_HOST
./bin/secure_keyed_join_test  --party=2 --storage=column --alice_host=$ALICE_HOST
./bin/secure_sort_merge_join_test --party=2 --storage=column --alice_host=$ALICE_HOST
./bin/secure_scalar_aggregate_test   --party=2 --storage=column --alice_host=$ALICE_HOST
./bin/secure_group_by_aggregate_test  --party=2 --storage=column --alice_host=$ALICE_HOST
./bin/secure_nested_loop_aggregate_test  --party=2 --storage=column --alice_host=$ALICE_HOST

#./bin/enrich_test  --party=2 --storage=column --alice_host=$ALICE_HOST # excluded because data is not synced between A + B
./bin/secure_tpch_test  --party=2 --storage=column --alice_host=$ALICE_HOST
