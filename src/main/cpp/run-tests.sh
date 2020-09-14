#!/bin/bash -x

./bin/value_expression_test

./bin/query_table_test

./bin/filter_test

./bin/project_test

./bin/sort_test


./bin/emp_manager_test --party=1 &
./bin/emp_manager_test --party=2

#sleep 5

./bin/secure_value_expression_test --party=1 &
./bin/secure_value_expression_test --party=2

./bin/secure_filter_test --party=1 &
./bin/secure_filter_test --party=2

#sleep 5

./bin/secure_sort_test --party=1 &
./bin/secure_sort_test --party=2



