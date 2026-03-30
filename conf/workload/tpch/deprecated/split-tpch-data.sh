#!/bin/bash -x

CONF_PATH=$(pwd)/conf/workload/tpch
TPCH_SCRIPTS_PATH=$(pwd)/deps/pg_tpch/dss


#place data where the loader expects it
rm /tmp/dss/data
cp -r $CONF_PATH/data /tmp/dss/data

#split TPC-H instance into two equal-sized halves
for TABLE in customer lineitem nation orders part  partsupp region supplier;
do
    FILE=/tmp/dss/data/$TABLE.csv
    FILE_LINES=$(wc -l <${FILE})
    LINES_PER_PARTY=$((($FILE_LINES + 1) /2))
    split -a 1 -l $LINES_PER_PARTY $FILE /tmp/dss/data/$TABLE.
done



dropdb tpch_unioned
createdb tpch_unioned
psql tpch_unioned < $TPCH_SCRIPTS_PATH/tpch-load.sql
psql tpch_unioned < $TPCH_SCRIPTS_PATH/tpch-pkeys.sql
psql tpch_unioned < $TPCH_SCRIPTS_PATH/tpch-alter.sql
psql tpch_unioned < $TPCH_SCRIPTS_PATH/tpch-index.sql

# now rename alice's halves into *.csv files
for TABLE in customer lineitem nation orders part  partsupp region supplier;
do
    mv /tmp/dss/data/$TABLE.a /tmp/dss/data/$TABLE.csv
done

dropdb tpch_alice
createdb tpch_alice
psql tpch_alice < $TPCH_SCRIPTS_PATH/tpch-load.sql
psql tpch_alice < $TPCH_SCRIPTS_PATH/tpch-pkeys.sql
psql tpch_alice < $TPCH_SCRIPTS_PATH/tpch-alter.sql
psql tpch_alice < $TPCH_SCRIPTS_PATH/tpch-index.sql