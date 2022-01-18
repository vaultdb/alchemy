
bash pilot/test/generate-and-load-test-data.sh 100

#warmup
#./bin/load_tuples_data_partner 127.0.0.1 54321 1 pilot/test/input/alice-patient.csv  pilot/test/output/chi-patient.alice &
#./bin/load_tuples_data_partner 127.0.0.1 54321 2 pilot/test/input/bob-patient.csv  pilot/test/output/chi-patient.bob


#query pipeline
#./bin/run_data_partner 127.0.0.1 54321 1 pilot/test/input/alice-patient.csv  pilot/test/output/chi-patient.alice  &
#./bin/run_data_partner 127.0.0.1 54321 2 pilot/test/input/bob-patient.csv  pilot/test/output/chi-patient.bob

./bin/run_data_partner -h 127.0.0.1 -P 54321 --party=1 -d enrich_htn_unioned_alice --rp=pilot/secret_shares/tables/partial_counts_site_3.alice  --rc=pilot/secret_shares/output/chi-patient.alice & 

./bin/run_data_partner -h 127.0.0.1 -P 54321 --party=2 -d enrich_htn_unioned_bob --rp=pilot/secret_shares/tables/partial_counts_site_3.bob  


./bin/assemble_secret_shares pilot/secret_shares/xor pilot/secret_shares/revealed
