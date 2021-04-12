
bash pilot/test/generate-and-load-test-data.sh 100

#warmup
#./bin/load_tuples_data_partner 127.0.0.1 54321 1 pilot/test/input/alice-patient.csv  pilot/test/output/chi-patient.alice &
#./bin/load_tuples_data_partner 127.0.0.1 54321 2 pilot/test/input/bob-patient.csv  pilot/test/output/chi-patient.bob


#query pipeline
./bin/run_data_partner 127.0.0.1 54321 1 pilot/test/input/alice-patient.csv  pilot/test/output/chi-patient.alice  &
./bin/run_data_partner 127.0.0.1 54321 2 pilot/test/input/bob-patient.csv  pilot/test/output/chi-patient.bob

./bin/assemble_secret_shares pilot/secret_shares/xor pilot/secret_shares/revealed
