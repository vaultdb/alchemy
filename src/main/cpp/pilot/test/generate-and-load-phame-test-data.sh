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

#clear out old data
rm pilot/secret-shares/input/*/*

make generate_phame_data_n_parties
./bin/generate_phame_data_n_parties pilot/test/input $HOST_COUNT $TUPLE_COUNT


dropdb  --if-exists $DB_NAME
createdb $DB_NAME


#parties 1, 3 are row-level partners
# they have site_ids stripped in the sql script below in order to simulate
# the format we're getting from SQL statements
psql $DB_NAME <  pilot/test/phame/load-generated-data.sql


# create schema files for each test CSV
for((i = 0; i < $HOST_COUNT; i++)); do
  echo '(patid:int32, dx_diabetes:bool, dx_hypertension:bool, dx_cervical_cancer:bool, dx_breast_cancer:bool, dx_lung_cancer:bool, dx_colorectal_cancer:bool)' > pilot/secret-shares/input/$i/phame_diagnosis_rollup.schema
  echo '(desc:varchar(42), count:int64, site_id:int32)' > pilot/secret-shares/input/$i/phame_cohort_counts.schema
done

for host in 1 3; do
  echo "Row-level schemas for " $host
  echo '(patid:int32, dx_diabetes:bool, dx_hypertension:bool, dx_cervical_cancer:bool, dx_breast_cancer:bool, dx_lung_cancer:bool, dx_colorectal_cancer:bool)' > pilot/secret-shares/input/$host/phame_diagnosis.schema
  echo '(patid:int32, age_cat:char(1), gender:char(1), race:char(1), ethnicity:char(1), zip:char(5), payer_primary:char(1), payer_secondary:char(1))' > pilot/secret-shares/input/$host/phame_demographic.schema
done
