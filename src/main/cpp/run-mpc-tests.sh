#!/bin/bash -x

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
