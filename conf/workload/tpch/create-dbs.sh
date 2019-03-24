#!/bin/bash -x


#run from smcql home, which contains src/                                                                                                                                            
CONF_PATH=$(pwd)/conf/workload/tpch
TPCH_SCRIPTS_PATH=$(pwd)/deps/pg_tpch/dss

echo "Using test data from $CONF_PATH"

#prereq: need a pg_dump of desired TPC-H configurations
#creates two copies and loads into two dbs, tpch_alice and tpch_bob
#default: TPC-H sf 0.01 = ~ 10 MB


#create alice in bob's DBs
for PARTY in alice bob
do
    DB_NAME='tpch_'$PARTY
    DB_FILE=$CONF_PATH/tpch-$PARTY.sql
    dropdb $DB_NAME
    createdb $DB_NAME
    psql $DB_NAME < $DB_FILE
done




#set up a unioned db containing all data 
# of alice and bob to verify our queries
dropdb tpch_unioned
createdb tpch_unioned
psql tpch_unioned < $CONF_PATH/tpch-alice.sql

# delete the primary key constraints to admit duplicates for unioned load
# then copy in bob's values
for TABLE in customer lineitem nation orders part  partsupp region supplier ; 
do 
    KEY=$TABLE'_pkey'; 
    psql tpch_unioned -c "ALTER TABLE $TABLE DROP CONSTRAINT $KEY  CASCADE"; 
    pg_dump -t $TABLE -a tpch_bob | psql tpch_unioned
done




#set up alice for fake distributed evaluation
# in the distributed setting we'd do this with Postgres FDW to create pointers to remote tables
psql tpch_alice < $CONF_PATH/tpch-create.sql

for TABLE in customer lineitem nation orders part partsupp region supplier    
do
       pg_dump -t $TABLE -a tpch_bob  | sed 's/COPY public./COPY public.remote_/' | psql tpch_alice
done
