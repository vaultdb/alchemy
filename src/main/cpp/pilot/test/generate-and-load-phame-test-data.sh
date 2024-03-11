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
mkdir -p pilot/secret-shares/input/0
mkdir pilot/secret-shares/input/1
mkdir pilot/secret-shares/input/2
mkdir pilot/secret-shares/input/3


make generate_phame_data_n_parties
./bin/generate_phame_data_n_parties pilot/test/input $HOST_COUNT $TUPLE_COUNT


dropdb  --if-exists $DB_NAME
createdb $DB_NAME


#parties 1, 3 are row-level partners
# they have site_ids stripped in the sql script below in order to simulate
# the format we're getting from SQL statements
psql $DB_NAME <  pilot/test/phame/load-generated-data.sql


