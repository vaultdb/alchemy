#!/bin/bash -x


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


./emp_test --party=1
./secure_field_expression_test	--party=1
./emp_table_test  --party=1
./secure_filter_test  --party=1
./secure_sort_test  --party=1
./secure_basic_join_test  --party=1
./secure_keyed_join_test  --party=1
./secure_scalar_aggregate_test 	 --party=1
./secure_group_by_aggregate_test  --party=1
#./enrich_test  --party=1  enrich test isn't synced yet on input data
./secure_tpch_test  --party=1


