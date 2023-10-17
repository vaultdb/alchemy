#!/bin/bash -x


cmake -DCMAKE_BUILD_TYPE=Release .
make -j

bash test/support/setup-csv.sh
bash pilot/test/generate-and-load-test-data.sh 1000
# regenerate test data
bash  test/support/load-generated-data.sh 100

# optional:
#  bash run-pilot-test.sh


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


./bin/emp_test --flagfile=flagfiles/alice.flags --storage=column --validation=false
./bin/secure_field_expression_test	--flagfile=flagfiles/alice.flags --storage=column --validation=false
./bin/emp_table_test  --flagfile=flagfiles/alice.flags --storage=column --validation=false
./bin/secure_filter_test  --flagfile=flagfiles/alice.flags --storage=column --validation=false
./bin/secure_sort_test  --flagfile=flagfiles/alice.flags --storage=column --validation=false
./bin/secure_basic_join_test  --cutoff=5 --storage=column --validation=false
./bin/secure_keyed_join_test  --flagfile=flagfiles/alice.flags --storage=column --validation=false
#./bin/secure_keyed_sort_merge_join_test --flagfile=flagfiles/alice.flags --storage=column
./bin/secure_scalar_aggregate_test 	 --flagfile=flagfiles/alice.flags --storage=column --validation=false
./bin/secure_sort_merge_aggregate_test  --flagfile=flagfiles/alice.flags --storage=column --validation=false
./bin/secure_nested_loop_aggregate_test --flagfile=flagfiles/alice.flags --storage=column --validation=false
#./bin/enrich_test  --party=1 --storage=column  #excluded because data is not synced between A + B
./bin/secure_plan_deparser_test --flagfile=flagfiles/alice.flags --storage=column --validation=false
./bin/secure_tpch_test  --flagfile=flagfiles/alice.flags --storage=column --validation=false
./bin/fully_optimized_test --flagfile=flagfiles/alice.flags --storage=column --validation=false
