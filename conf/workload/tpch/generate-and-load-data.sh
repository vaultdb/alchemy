#!/bin/bash -x

#starts from alchemy home (location of pom.xml)
CONF_PATH=$PWD/conf/workload/tpch
TPCH_SCRIPTS_PATH=$PWD/deps/pg_tpch/dss
TPCH_DATAGEN_PATH=$PWD/deps/tpch/dss

mkdir -p /tmp/dss/data/
rm -rf /tmp/dss/data/*




#construct unioned database first
#Generated case
if [ "$#" -ne 0 ]; then
    TPCH_SF=$1
    DB_NAME=tpch_unioned_sf$TPCH_SF
    ALICE_DB=tpch_alice_sf$TPCH_SF
    BOB_DB=tpch_bob_sf$TPCH_SF

    pushd deps/tpch/dbgen


    if [ -f dbgen ]; then
	./dbgen -s $TPCH_SF
	mv *.tbl /tmp/dss/data/
	for i in `ls /tmp/dss/data/*.tbl`; do 
	    sed 's/|$//' $i > ${i/tbl/csv}; 
	done;
    else
	echo "Build tpc-h generator first!" && exit -1
    fi

    popd #back to alchemy home



else  #default setup, SF = 0.1, use pre-generated files
    cp $CONF_PATH/data/*.csv /tmp/dss/data/
    DB_NAME=tpch_unioned
    ALICE_DB=tpch_alice
    BOB_DB=tpch_bob
fi


dropdb $DB_NAME
createdb $DB_NAME

psql $DB_NAME < $TPCH_SCRIPTS_PATH/tpch-load.sql
psql $DB_NAME < $TPCH_SCRIPTS_PATH/tpch-pkeys.sql
psql $DB_NAME < $TPCH_SCRIPTS_PATH/tpch-alter.sql
psql $DB_NAME < $TPCH_SCRIPTS_PATH/tpch-index.sql

#add permissions
psql $DB_NAME < $CONF_PATH/set-security-policy.sql


#OK, DB is set up, now need to partition for Alice and Bob

#Alice: nations 0-12
#Bob: nations 13-24

dropdb $ALICE_DB
createdb $ALICE_DB

dropdb $BOB_DB
createdb $BOB_DB



#create a deep copy of tpch_unioned for alice and bob and delete the records not in their partition
pg_dump tpch_unioned | psql $ALICE_DB
pg_dump tpch_unioned | psql $BOB_DB

psql $ALICE_DB < $CONF_PATH/select-alice.sql
psql $BOB_DB < $CONF_PATH/select-bob.sql


