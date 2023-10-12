#!/bin/bash -x


cmake -DCMAKE_BUILD_TYPE=Release .
make -j

bash test/support/setup-csv.sh
bash pilot/test/generate-and-load-test-data.sh 1000
# regenerate test data
bash  test/support/load-generated-data.sh 100

./bin/csv_reader_test --flagfile=flagfiles/plain.flags  --storage=column
./bin/field_expression_test --flagfile=flagfiles/plain.flags  --storage=column
./bin/filter_test --flagfile=flagfiles/plain.flags  --storage=column
./bin/project_test --flagfile=flagfiles/plain.flags  --storage=column
./bin/basic_join_test --cutoff=5  --storage=column
./bin/keyed_join_test --flagfile=flagfiles/plain.flags  --storage=column
# SMJ for column store is not implemented yet
#./bin/keyed_sort_merge_join_test --flagfile=flagfiles/plain.flags
./bin/merge_join_test --flagfile=flagfiles/plain.flags  --storage=column
./bin/sort_test --flagfile=flagfiles/plain.flags  --storage=column
./bin/scalar_aggregate_test --flagfile=flagfiles/plain.flags  --storage=column
./bin/sort_merge_aggregate_test --flagfile=flagfiles/plain.flags  --storage=column
./bin/nested_loop_aggregate_test --flagfile=flagfiles/plain.flags  --storage=column
./bin/secret_share_generator_test  --flagfile=flagfiles/plain.flags --storage=column 
#this test depends on pilot testbed
./bin/serialization_test --flagfile=flagfiles/plain.flags  --storage=column
./bin/plan_parser_test --flagfile=flagfiles/plain.flags  --storage=column
./bin/plan_deparser_test --flagfile=flagfiles/plain.flags  --storage=column
./bin/tpch_test --flagfile=flagfiles/plain.flags  --storage=column

./bin/filter_test --flagfile=flagfiles/plain.flags --storage=column
./bin/project_test --flagfile=flagfiles/plain.flags --storage=column
./bin/basic_join_test --flagfile=flagfiles/plain.flags --storage=column
./bin/keyed_join_test --flagfile=flagfiles/plain.flags --storage=column


bash run-column-store.sh emp_test 
bash run-column-store.sh secure_field_expression_test  
bash run-column-store.sh emp_table_test 
bash run-column-store.sh secure_filter_test 
bash run-column-store.sh secure_sort_test 
./bin/secure_basic_join_test --cutoff=5 --storage=column &
./bin/secure_basic_join_test --party=2 --cutoff=5 --storage=column
bash run-column-store.sh secure_keyed_join_test 
#bash run-column-store.sh secure_keyed_sort_merge_join_test
bash run-column-store.sh secure_scalar_aggregate_test
bash run-column-store.sh secure_sort_merge_aggregate_test
bash run-column-store.sh secure_nested_loop_aggregate_test
bash run-column-store.sh enrich_test
bash run-column-store.sh secure_plan_deparser_test
bash run-column-store.sh secure_tpch_test
bash run-column-store.sh fully_optimized_test

# ZK Tests -- these are optional
#bash run-column-store.sh zk_test
#bash run-column-store.sh zk_filter_test
#bash run-column-store.sh  zk_basic_join_test
#bash run-column-store.sh zk_keyed_join_test
#bash run-column-store.sh  zk_scalar_aggregate_test
#bash run-column-store.sh zk_sort_merge_aggregate_test
#bash run-column-store.sh zk_sort_test
bash run-column-store.sh zk_tpch_test

