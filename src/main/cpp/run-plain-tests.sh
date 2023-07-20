#!/bin/bash
set +x


./bin/csv_reader_test --flagfile=flagfiles/plain.flags
./bin/field_expression_test --flagfile=flagfiles/plain.flags
./bin/filter_test --flagfile=flagfiles/plain.flags
./bin/project_test --flagfile=flagfiles/plain.flags
./bin/basic_join_test --cutoff=5
./bin/keyed_join_test --flagfile=flagfiles/plain.flags
./bin/sort_merge_join_test --flagfile=flagfiles/plain.flags
./bin/merge_join_test --flagfile=flagfiles/plain.flags
./bin/sort_test --flagfile=flagfiles/plain.flags
./bin/scalar_aggregate_test --flagfile=flagfiles/plain.flags
./bin/group_by_aggregate_test --flagfile=flagfiles/plain.flags
./bin/nested_loop_aggregate_test --flagfile=flagfiles/plain.flags
./bin/secret_share_generator_test  --flagfile=flagfiles/plain.flags
#this test depends on pilot testbed
./bin/serialization_test --flagfile=flagfiles/plain.flags
./bin/plan_parser_test --flagfile=flagfiles/plain.flags
./bin/tpch_test --flagfile=flagfiles/plain.flags
