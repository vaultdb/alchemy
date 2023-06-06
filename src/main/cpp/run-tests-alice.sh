#!/bin/bash -x


cmake -DCMAKE_BUILD_TYPE=Release .
make -j

bash test/support/setup-csv.sh
bash pilot/test/generate-and-load-test-data.sh 1000
# regenerate test data
bash  test/support/load-generated-data.sh 100

# optional:
#  bash run-pilot-test.sh


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


./bin/emp_test --party=1
./bin/secure_field_expression_test	--party=1
./bin/emp_table_test  --party=1
./bin/secure_filter_test  --party=1
./bin/secure_sort_test  --party=1
./bin/secure_basic_join_test  --party=1
./bin/secure_keyed_join_test  --party=1
./bin/secure_scalar_aggregate_test 	 --party=1
./bin/secure_group_by_aggregate_test  --party=1
./bin/enrich_test  --party=1
./bin/secure_tpch_test  --party=1


