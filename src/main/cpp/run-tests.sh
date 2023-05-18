#!/bin/bash -x


cmake .
make -j5

bash test/support/setup-csv.sh
bash pilot/test/generate-and-load-test-data.sh 1000
# regenerate test data
bash  test/support/load-generated-data.sh 100

# optional:
#  bash run-pilot-test.sh

#mkdir log

./csv_reader_test 
./field_expression_test
./filter_test
./project_test
./basic_join_test
./fkey_pkey_join_test
./sort_test
./scalar_aggregate_test
./group_by_aggregate_test
./secret_share_generator_test 
#this test depends on pilot testbed
./serialization_test
./plan_parser_test
./tpch_test

bash run.sh emp_test
bash run.sh secure_field_expression_test
bash run.sh emp_table_test
bash run.sh secure_filter_test
bash run.sh secure_sort_test
bash run.sh secure_basic_join_test
bash run.sh secure_pkey_fkey_join_test
bash run.sh secure_scalar_aggregate_test
bash run.sh secure_group_by_aggregate_test
bash run.sh enrich_test
bash run.sh secure_tpch_test

# ZK Tests -- these are optional
#bash run.sh zk_test
#bash run.sh zk_filter_test
#bash run.sh  zk_basic_join_test
#bash run.sh zk_keyed_join_test
#bash run.sh  zk_scalar_aggregate_test
#bash run.sh zk_group_by_aggregate_test
#bash run.sh zk_sort_test
#bash run.sh zk_tpch_test

