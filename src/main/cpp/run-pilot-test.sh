#!/bin/bash

make -j4
bash pilot/test/generate-and-load-test-data.sh 1000

# main test
./bin/run_data_partner -h 127.0.0.1 -P 54321 --party=1 -d enrich_htn_alice -p pilot/secret_shares/tables/chi_partial_counts.alice -r pilot/test/output/chi-patient-multisite.alice -y all -s &

./bin/run_data_partner -h 127.0.0.1 -P 54321 --party=2 -d enrich_htn_bob -p pilot/secret_shares/tables/chi_partial_counts.bob -r pilot/test/output/chi-patient-multisite.bob -y all -s


sleep 2
#batch test
./bin/run_data_partner_batch -h 127.0.0.1 -P 54321 --party=1 -d enrich_htn_alice -r pilot/test/batch/chi-patient -y all --batch-count 10  &

./bin/run_data_partner_batch  -h 127.0.0.1 -P 54321 --party=2 -d enrich_htn_bob -r pilot/test/batch/chi-patient -y all --batch-count 10

sleep 2

#aggregate only test
./bin/run_data_partner_aggregation 127.0.0.1 54321 1 enrich_htn_alice all pilot/secret_shares/tables/chi_counts.alice &
./bin/run_data_partner_aggregation 127.0.0.1 54321 2 enrich_htn_bob all pilot/secret_shares/tables/chi_counts.bob
