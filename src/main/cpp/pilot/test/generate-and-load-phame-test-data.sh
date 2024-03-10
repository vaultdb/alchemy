#!/bin/bash -x

if [ "$#" -ne 2 ]; then
    echo "usage: pilot/test/generate-and-load-phame-data.sh <host count> <tuple count per host>"
    exit
fi

HOST_COUNT=$1
TUPLE_COUNT=$2
DB_NAME='phame_unioned'

mkdir -p pilot/test/input
mkdir -p pilot/test/output

make generate_phame_data_n_parties
./bin/generate_phame_data_n_parties pilot/test/input $HOST_COUNT $TUPLE_COUNT


dropdb  --if-exists $DB_NAME
createdb $DB_NAME


psql $DB_NAME <  pilot/test/phame/load-generated-data.sql
