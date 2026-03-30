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
bash pilot/test/generate-and-load-enrich-test-data.sh 1000
# regenerate test data
bash  test/support/load-generated-enrich-data.sh 100

bash scripts/run-plain-tests.sh


ALICE_HOST=$1
./bin/emp_test --flagfile=flagfiles/bob.flags --alice_host=$ALICE_HOST --validation=false
./bin/secure_field_expression_test  --flagfile=flagfiles/bob.flags --alice_host=$ALICE_HOST --validation=false
./bin/emp_table_test  --flagfile=flagfiles/bob.flags --alice_host=$ALICE_HOST --validation=false
./bin/secure_filter_test --flagfile=flagfiles/bob.flags --alice_host=$ALICE_HOST --validation=false
./bin/secure_sort_test  --flagfile=flagfiles/bob.flags --alice_host=$ALICE_HOST --validation=false
./bin/secure_basic_join_test  --party=2  --cutoff=5  --alice_host=$ALICE_HOST --validation=false
./bin/secure_keyed_join_test  --flagfile=flagfiles/bob.flags --alice_host=$ALICE_HOST --validation=false
./bin/secure_keyed_sort_merge_join_test --flagfile=flagfiles/bob.flags --alice_host=$ALICE_HOST --validation=false
./bin/secure_merge_join_test  --flagfile=flagfiles/bob.flags --alice_host=$ALICE_HOST --validation=false
./bin/secure_scalar_aggregate_test   --flagfile=flagfiles/bob.flags --alice_host=$ALICE_HOST --validation=false
./bin/secure_sort_merge_aggregate_test  --flagfile=flagfiles/bob.flags --alice_host=$ALICE_HOST --validation=false
./bin/secure_nested_loop_aggregate_test --flagfile=flagfiles/bob.flags --alice_host=$ALICE_HOST --validation=false
#./bin/enrich_test  --party=2 --alice_host=$ALICE_HOST # excluded because data is not synced between A + B
./bin/secure_tpch_test  --flagfile=flagfiles/bob.flags --alice_host=$ALICE_HOST --validation=false
./bin/fully_optimized_test  --flagfile=flagfiles/bob.flags --alice_host=$ALICE_HOST --validation=false
