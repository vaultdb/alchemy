#!/bin/bash

./pilot/prod/load/alliance/clean.sh

mkdir pilot/input/clean
mkdir pilot/secret_shares/
mkdir pilot/secret_shares/
mkdir pilot/secret_shares/

dropdb enrich_htn_prod
createdb enrich_htn_prod

psql enrich_htn_prod <  pilot/prod/load/load-data-alliance.sql

./bin/secret_share_csv pilot/input/clean/2018.csv pilot/secret_shares/2018
./bin/secret_share_csv pilot/input/clean/2019.csv pilot/secret_shares/2019
./bin/secret_share_csv pilot/input/clean/2020.csv pilot/secret_shares/2020
./bin/secret_share_csv pilot/input/clean/all.csv pilot/secret_shares/all

