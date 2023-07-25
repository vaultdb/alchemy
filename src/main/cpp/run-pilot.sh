
bash pilot/test/generate-and-load-test-data.sh 1000

make -j run_data_partner assemble_secret_shares

mkdir -p pilot/test/batch/
mkdir -p pilot/secret_shares/tables

#warmup
#./bin/load_tuples_data_partner 127.0.0.1 54321 1 pilot/test/input/alice-patient.csv  pilot/test/output/chi-patient.alice &
#./bin/load_tuples_data_partner 127.0.0.1 54321 2 pilot/test/input/bob-patient.csv  pilot/test/output/chi-patient.bob


#query pipeline
#./bin/run_data_partner 127.0.0.1 54321 1 pilot/test/input/alice-patient.csv  pilot/test/output/chi-patient.alice  &
#./bin/run_data_partner 127.0.0.1 54321 2 pilot/test/input/bob-patient.csv  pilot/test/output/chi-patient.bob

#main test
./bin/run_data_partner -h 127.0.0.1 -P 54321 --party=1 -d enrich_htn_alice -p pilot/secret_shares/tables/chi_partial_counts.alice -r pilot/test/output/chi-patient-multisite.alice -y all -s &


./bin/run_data_partner -h 127.0.0.1 -P 54321 --party=2 -d enrich_htn_bob -p pilot/secret_shares/tables/chi_partial_counts.bob -r pilot/test/output/chi-patient-multisite.bob -y all -s


./bin/assemble_secret_shares pilot/secret_shares/xor pilot/secret_shares/revealed


