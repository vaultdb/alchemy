#!/bin/bash
set +x


./bin/csv_reader_test
./bin/field_expression_test
./bin/filter_test
./bin/project_test
./bin/basic_join_test
./bin/keyed_join_test
./bin/sort_test
./bin/scalar_aggregate_test
./bin/group_by_aggregate_test
./bin/nested_loop_aggregate_test
./bin/secret_share_generator_test 
#this test depends on pilot testbed
./bin/serialization_test
./bin/plan_parser_test
./bin/tpch_test
