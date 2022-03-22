# everything we need to make the unit tests run

make -j5
test/support/setup-csv.sh

#enrich test
test/support/load-generated-data.sh 100

#set up truncated DBs for shorter tests
psql tpch_unioned <  truncate-tpch.sql

mkdir -p pilot/secret_shares/tables
mkdir -p pilot/test/batch
