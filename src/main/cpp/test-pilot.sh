
bash pilot/prod/clean-nulls.sh 
mkdir pilot/input/2018
mkdir pilot/input/2019
mkdir pilot/input/2020


dropdb enrich_htn_prod


createdb enrich_htn_prod
psql enrich_htn_prod -v ON_ERROR_STOP=on < pilot/prod/load-data-nm.sql

./bin/secret_share_csv pilot/input/chi-prod-patient.csv pilot/secret_shares/chi-prod-patient

#warmup
./bin/load_tuples_data_partner 127.0.0.1 54321 1 pilot/input/alice-prod-patient.csv  pilot/secret_shares/chi-prod-patient.alice &                                                                                                                          
./bin/load_tuples_data_partner 127.0.0.1 54321 2 pilot/input/bob-prod-patient.csv   pilot/secret_shares/chi-prod-patient.bob                                                                                                                                         


