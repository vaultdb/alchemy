#!/bin/bash -x


cmake -DCMAKE_BUILD_TYPE=Release .
make -j

bash test/support/setup-csv.sh
bash pilot/test/generate-and-load-test-data.sh 1000
# regenerate test data
bash  test/support/load-generated-data.sh 100

# optional:
#  bash run-pilot-test.sh


bash scripts/run-plain-tests.sh


./bin/emp_test --flagfile=flagfiles/alice.flags --validation=false
./bin/secure_field_expression_test	--flagfile=flagfiles/alice.flags --validation=false
./bin/emp_table_test  --flagfile=flagfiles/alice.flags --validation=false
./bin/secure_filter_test  --flagfile=flagfiles/alice.flags --validation=false
./bin/secure_sort_test  --flagfile=flagfiles/alice.flags --validation=false
./bin/secure_basic_join_test  --cutoff=5 --validation=false
./bin/secure_keyed_join_test  --flagfile=flagfiles/alice.flags --validation=false
./bin/secure_keyed_sort_merge_join_test --flagfile=flagfiles/alice.flags --validation=false
./bin/secure_merge_join_test --flagfile=flagfiles/alice.flags --validation=false
./bin/secure_scalar_aggregate_test 	 --flagfile=flagfiles/alice.flags --validation=false
./bin/secure_sort_merge_aggregate_test  --flagfile=flagfiles/alice.flags --validation=false
./bin/secure_nested_loop_aggregate_test --flagfile=flagfiles/alice.flags --validation=false
#./bin/enrich_test  --party=1  #excluded because data is not synced between A + B
./bin/secure_tpch_test  --flagfile=flagfiles/alice.flags --validation=false
./bin/fully_optimized_test  --flagfile=flagfiles/alice.flags --validation=false

