#!/bin/bash -x


cmake -DCMAKE_BUILD_TYPE=Release .
make -j

bash test/support/setup-csv.sh
bash pilot/test/generate-and-load-test-data.sh 1000
# regenerate test data
bash  test/support/load-generated-data.sh 100

# optional:
#  bash run-pilot-test.sh


bash run-plain-tests.sh

bash run.sh emp_test 
bash run.sh secure_field_expression_test  
bash run.sh emp_table_test 
bash run.sh secure_filter_test 
bash run.sh secure_sort_test 
bash run.sh secure_basic_join_test 
bash run.sh secure_keyed_join_test 
bash run.sh secure_sort_merge_join_test
bash run.sh secure_scalar_aggregate_test
bash run.sh secure_group_by_aggregate_test 
bash run.sh secure_nested_loop_aggregate_test
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

