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

./bin/csv_reader_test --flagfile=flagfiles/plain.flags --storage=column
./bin/field_expression_test --flagfile=flagfiles/plain.flags --storage=column
./bin/filter_test --flagfile=flagfiles/plain.flags  --storage=column
./bin/project_test --flagfile=flagfiles/plain.flags  --storage=column
./bin/basic_join_test --flagfile=flagfiles/plain.flags --storage=column
./bin/keyed_join_test --flagfile=flagfiles/plain.flags --storage=column
./bin/keyed_sort_merge_join_test --flagfile=flagfiles/plain.flags --storage=column
./bin/sort_test --flagfile=flagfiles/plain.flags --storage=column
./bin/scalar_aggregate_test --flagfile=flagfiles/plain.flags --storage=column
./bin/sort_merge_aggregate_test --flagfile=flagfiles/plain.flags --storage=column
./bin/nested_loop_aggregate_test --flagfile=flagfiles/plain.flags --storage=column
./bin/secret_share_generator_test --flagfile=flagfiles/plain.flags --storage=column
#this test depends on pilot testbed
./bin/serialization_test --flagfile=flagfiles/plain.flags --storage=column
./bin/plan_parser_test --flagfile=flagfiles/plain.flags --storage=column
./bin/tpch_test --flagfile=flagfiles/plain.flags --storage=column


ALICE_HOST=$1
./bin/emp_test  --flagfile=flagfiles/bob.flags --storage=column --alice_host=$ALICE_HOST
./bin/secure_field_expression_test  --flagfile=flagfiles/bob.flags --storage=column --alice_host=$ALICE_HOST
./bin/emp_table_test  --flagfile=flagfiles/bob.flags --storage=column --alice_host=$ALICE_HOST
./bin/secure_filter_test  --flagfile=flagfiles/bob.flags --storage=column --alice_host=$ALICE_HOST
./bin/secure_sort_test  --flagfile=flagfiles/bob.flags --storage=column --alice_host=$ALICE_HOST
./bin/secure_basic_join_test  --flagfile=flagfiles/bob.flags --storage=column --alice_host=$ALICE_HOST
./bin/secure_keyed_join_test  --flagfile=flagfiles/bob.flags --storage=column --alice_host=$ALICE_HOST
./bin/secure_keyed_sort_merge_join_test --flagfile=flagfiles/bob.flags --storage=column --alice_host=$ALICE_HOST
./bin/secure_scalar_aggregate_test   --flagfile=flagfiles/bob.flags --storage=column --alice_host=$ALICE_HOST
./bin/secure_sort_merge_aggregate_test  --flagfile=flagfiles/bob.flags --storage=column --alice_host=$ALICE_HOST
./bin/secure_nested_loop_aggregate_test  --flagfile=flagfiles/bob.flags --storage=column --alice_host=$ALICE_HOST

#./bin/enrich_test  --party=2 --storage=column --alice_host=$ALICE_HOST # excluded because data is not synced between A + B
./bin/secure_tpch_test  --flagfile=flagfiles/bob.flags --storage=column --alice_host=$ALICE_HOST
