#!/bin/bash -x


make -j5
./bin/value_expression_test && \
./bin/query_table_test && \
./bin/filter_test && \
./bin/project_test && \
./bin/sort_test && \
./bin/basic_join_test && \
./bin/fkey_pkey_join_test && \
    ./bin/scalar_aggregate_test && \
    ./bin/group_by_aggregate_test && \
./bin/csv_reader_test


#pairs for Alice and Bob
./bin/emp_test --party=1 & 
./bin/emp_test  --party=2

./bin/emp_table_test --party=1 &
./bin/emp_table_test  --party=2


./bin/secure_value_expression_test --party=1 &
./bin/secure_value_expression_test --party=2

./bin/secure_filter_test --party=1 &
./bin/secure_filter_test --party=2


./bin/secure_sort_test --party=1 &
./bin/secure_sort_test --party=2


./bin/secure_basic_join_test --party=1 &
./bin/secure_basic_join_test --party=2


./bin/secure_pkey_fkey_join_test --party=1 &
./bin/secure_pkey_fkey_join_test --party=2

./bin/secure_scalar_aggregate_test --party=1 &
./bin/secure_scalar_aggregate_test --party=2

./bin/secure_group_by_aggregate_test --party=1 &
./bin/secure_group_by_aggregate_test --party=2


./bin/emp_float_to_int --party=1 &
./bin/emp_float_to_int --party=2 
