#!/bin/bash
set +x


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
