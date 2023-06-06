#!/bin/bash -x


cmake -DCMAKE_BUILD_TYPE=Release .
make -j

bash test/support/setup-csv.sh
bash pilot/test/generate-and-load-test-data.sh 1000
# regenerate test data
bash  test/support/load-generated-data.sh 100

# optional:
#  bash run-pilot-test.sh

#VG = passed valgrind as of 6/6/23
./bin/csv_reader_test #VG
./bin/field_expression_test #VG
./bin/filter_test  #VG
./bin/project_test  #VG
./bin/basic_join_test #VG
./bin/keyed_join_test #VG
./bin/sort_test #VG
./bin/scalar_aggregate_test #VG
./bin/group_by_aggregate_test #VG
./bin/secret_share_generator_test #VG
#this test depends on pilot testbed
./bin/serialization_test #VG
./bin/plan_parser_test #VG
./bin/tpch_test #VG

bash run.sh emp_test #VG
bash run.sh secure_field_expression_test  ##VG
bash run.sh emp_table_test #VG
bash run.sh secure_filter_test #VG
bash run.sh secure_sort_test #VG
bash run.sh secure_basic_join_test #VG
bash run.sh secure_keyed_join_test #VG
bash run.sh secure_scalar_aggregate_test # VG
bash run.sh secure_group_by_aggregate_test #VG
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

