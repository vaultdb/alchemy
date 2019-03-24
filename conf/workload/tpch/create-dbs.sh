#!/bin/bash -x


#run from smcql home, which contains src/                                                                                                                                            
LOCAL_PATH=$(pwd)/conf/workload/tpch
echo "Using test data from $LOCAL_PATH"

#prereq: need a pg_dump of desired TPC-H configuration
#creates two copies and loads into two dbs, tpch_alice and tpch_bob
#default: TPC-H sf 0.01 = ~ 10 MB

TPCH_FILE=$LOCAL_PATH'/tpch-load.sql' 
if [[ ! -f $TPCH_FILE ]] ; then
    echo 'File '$TPCH_FILE' is not there, create a tpc-h file first.'
    exit
fi


#create two identical DBs
for PARTY in alice bob
do
    DB_NAME='tpch_'$PARTY
    dropdb $DB_NAME
    createdb $DB_NAME
    psql $DB_NAME < $TPCH_FILE
done

#set up alice for fake distributed evaluation
for TABLE in customer lineitem nation orders part partsupp region supplier    
do
    psql tpch_alice -c "SELECT * INTO remote_$TABLE  FROM $TABLE"
done
