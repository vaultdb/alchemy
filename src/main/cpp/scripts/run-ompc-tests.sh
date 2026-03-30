#!/bin/bash -x


RUNNER='scripts/run-ompc.sh'

bash $RUNNER ompc_emp_test
bash $RUNNER ompc_field_expression_test
bash $RUNNER ompc_emp_table_test
bash $RUNNER ompc_stored_table_test
bash $RUNNER ompc_filter_test
bash $RUNNER ompc_project_test
bash $RUNNER ompc_basic_join_test
bash $RUNNER ompc_keyed_join_test
bash $RUNNER ompc_sort_test
bash $RUNNER ompc_block_nested_loop_join_test
bash $RUNNER ompc_keyed_sort_merge_join_test
bash $RUNNER ompc_scalar_aggregate_test
bash $RUNNER ompc_sort_merge_aggregate_test
bash $RUNNER ompc_nested_loop_aggregate_test
bash $RUNNER ompc_tpch_test
