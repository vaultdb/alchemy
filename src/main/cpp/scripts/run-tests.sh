#!/bin/bash -x


cmake -DCMAKE_BUILD_TYPE=Release .
make -j4

bash test/support/setup-csv.sh
bash pilot/test/generate-and-load-test-data.sh 1000
# regenerate test data
bash  test/support/load-generated-data.sh 100



# can speed up with (4 threads):
# parallel -j 4 < run-plain-tests.sh
#use `sudo apt install parallel` to set this up in ubuntu
bash scripts/run-plain-tests.sh
bash scripts/run-mpc-tests.sh

# ZK Tests -- these are optional
#bash run.sh zk_test
#bash run.sh zk_filter_test
#bash run.sh  zk_basic_join_test
#bash run.sh zk_keyed_join_test
#bash run.sh  zk_scalar_aggregate_test
#bash run.sh zk_sort_merge_aggregate_test
#bash run.sh zk_sort_test
bash scripts/run.sh zk_tpch_test

#bash run-pilot-test.sh
./bin/run_data_partner -h 127.0.0.1 -P 54321 --party=1 -d enrich_htn_alice -p pilot/secret_shares/tables/chi_partial_counts.alice -r pilot/test/output/chi-patient-multisite.alice -y all -s &
./bin/run_data_partner -h 127.0.0.1 -P 54321 --party=2 -d enrich_htn_bob -p pilot/secret_shares/tables/chi_partial_counts.bob -r pilot/test/output/chi-patient-multisite.bob -y all -s
./bin/assemble_secret_shares pilot/secret_shares/xor pilot/secret_shares/revealed
