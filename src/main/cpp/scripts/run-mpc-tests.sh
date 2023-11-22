#!/bin/bash -x

RUNNER='scripts/run.sh'
#RUNNER='scripts/run-ompc.sh'

bash $RUNNER emp_test
bash $RUNNER secure_field_expression_test
bash $RUNNER emp_table_test
bash $RUNNER secure_filter_test
bash $RUNNER secure_sort_test

#manually running this to manage cutoff
./bin/secure_basic_join_test --cutoff=5 --flagfile=flagfiles/filter.flags --flagfile=flagfiles/db_names.flags --flagfile=flagfiles/storage.flags & 
./bin/secure_basic_join_test --party=2 --cutoff=5  --flagfile=flagfiles/filter.flags --flagfile=flagfiles/db_names.flags --flagfile=flagfiles/storage.flags 

bash $RUNNER secure_keyed_join_test
bash $RUNNER secure_keyed_sort_merge_join_test
bash $RUNNER secure_merge_join_test
bash $RUNNER secure_scalar_aggregate_test
bash $RUNNER secure_sort_merge_aggregate_test
bash $RUNNER secure_nested_loop_aggregate_test
bash $RUNNER enrich_test #omitted from OMPC because inputs are wired differently
bash $RUNNER secure_plan_deparser_test
bash $RUNNER secure_tpch_test
bash $RUNNER fully_optimized_test
