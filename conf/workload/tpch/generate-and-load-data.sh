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
    DB_NAME=tpch_unioned #_sf$TPCH_SF -- just put them all in the same place
    ALICE_DB=tpch_alice #_sf$TPCH_SF
    BOB_DB=tpch_bob #_sf$TPCH_SF

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


dropdb --if-exists $DB_NAME
createdb $DB_NAME

psql $DB_NAME < $TPCH_SCRIPTS_PATH/tpch-load.sql
psql $DB_NAME < $TPCH_SCRIPTS_PATH/tpch-alter.sql
psql $DB_NAME < $TPCH_SCRIPTS_PATH/tpch-index.sql

#add permissions
psql $DB_NAME < $CONF_PATH/set-security-policy.sql



#OK, DB is set up, now need to partition for Alice and Bob

#Alice: nations 0-12
#Bob: nations 13-24

dropdb --if-exists $ALICE_DB
createdb $ALICE_DB

dropdb --if-exists $BOB_DB
createdb $BOB_DB



#create a deep copy of tpch_unioned for alice and bob and delete the records not in their partition
pg_dump $DB_NAME | psql $ALICE_DB
pg_dump $DB_NAME | psql $BOB_DB

psql $ALICE_DB < $CONF_PATH/select-alice.sql
psql $BOB_DB < $CONF_PATH/select-bob.sql


#add constraints
psql $DB_NAME < $CONF_PATH/set-constraints.sql
psql $ALICE_DB < $CONF_PATH/set-constraints.sql
psql $BOB_DB < $CONF_PATH/set-constraints.sql


# optional -- use this to populate main dbs

#if [ "$#" -ne 0 ]; then
#rename to main DBs
#    dropdb tpch_alice
#    dropdb tpch_bob
#    dropdb tpch_unioned

#    CMD='ALTER DATABASE \"' $ALICE_DB '\" RENAME TO tpch_alice'
#    psql -c "$CMD"

#    CMD='ALTER DATABASE \"' $BOB_DB '\" RENAME TO tpch_bob'
#    psql -c "$CMD"

#    CMD='ALTER DATABASE \"' $DB_NAME '\" RENAME TO tpch_unioned'
#    psql -c "$CMD"
#fi


