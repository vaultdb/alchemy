#!/bin/bash -x

#example usage:
# bash pilot/test/generate-and-load-phame-data.sh 4 100
if [ "$#" -ne 2 ]; then
    echo "usage: pilot/test/generate-and-load-phame-data.sh <host count> <tuple count per host>"
    exit
fi


HOST_COUNT=$1
TUPLE_COUNT=$2
DB_NAME='phame_unioned'

mkdir -p pilot/test/input
mkdir -p pilot/test/output
mkdir -p pilot/secret_shares/input/0
mkdir pilot/secret_shares/input/1
mkdir pilot/secret_shares/input/2
mkdir pilot/secret_shares/input/3


#clear out old data
rm pilot/secret_shares/input/*/*

make generate_phame_data_n_parties
./bin/generate_phame_data_n_parties pilot/test/input $HOST_COUNT $TUPLE_COUNT


dropdb  --if-exists $DB_NAME
createdb $DB_NAME

mkdir -p pilot/study/phame/expected
echo '(desc:varchar(42), count:int64)' > pilot/study/phame/expected/phame_cohort_counts.schema
echo '(age_cat:char(1), gender:char(1), race:char(1), ethnicity:char(1), zip:char(5), payer_primary:char(1), payer_secondary:char(1), patient_cnt:int64, diabetes_cnt:int64, hypertension_cnt:int64, cervical_cancer_cnt:int64, breast_cancer_cnt:int64, lung_cancer_cnt:int64, colorectal_cancer_cnt:int64)' > pilot/study/phame/expected/phame_diagnosis_rollup.schema

#parties 1, 3 are row-level partners
# they have site_ids stripped in the sql script below in order to simulate
# the format we're getting from SQL statements
psql $DB_NAME <  pilot/test/phame/load-generated-data.sql


# create schema files for each test CSV
for((i = 0; i < $HOST_COUNT; i++)); do
  echo '(age_cat:char(1), gender:char(1), race:char(1), ethnicity:char(1), zip:char(5), payer_primary:char(1), payer_secondary:char(1), patient_cnt:int64, diabetes_cnt:int64, hypertension_cnt:int64, cervical_cancer_cnt:int64, breast_cancer_cnt:int64, lung_cancer_cnt:int64, colorectal_cancer_cnt:int64)' > pilot/secret_shares/input/$i/phame_diagnosis_rollup.schema
  echo '(desc:varchar(42), count:int64, site_id:int32)' > pilot/secret_shares/input/$i/phame_cohort_counts.schema
done

for host in 1 3; do
  echo '(patid:int32, dx_diabetes:bool, dx_hypertension:bool, dx_cervical_cancer:bool, dx_breast_cancer:bool, dx_lung_cancer:bool, dx_colorectal_cancer:bool)' > pilot/secret_shares/input/$host/phame_diagnosis.schema
  echo '(patid:int32, age_cat:char(1), gender:char(1), race:char(1), ethnicity:char(1), zip:char(5), payer_primary:char(1), payer_secondary:char(1))' > pilot/secret_shares/input/$host/phame_demographic.schema
done


### Generate the secret shares
mkdir -p pilot/secret_shares/output/0
mkdir pilot/secret_shares/output/1
mkdir pilot/secret_shares/output/2
mkdir pilot/secret_shares/output/3
#clear out old data
rm pilot/secret_shares/output/*/*


for((i = 0; i < $HOST_COUNT; i++)); do
  ./bin/secret_share_csv pilot/secret_shares/input/$i/phame_cohort_counts.csv   pilot/secret_shares/input/$i/phame_cohort_counts.schema pilot/secret_shares/output/$i/phame_cohort_counts
  ./bin/secret_share_csv pilot/secret_shares/input/$i/phame_diagnosis_rollup.csv pilot/secret_shares/input/$i/phame_diagnosis_rollup.schema pilot/secret_shares/output/$i/phame_diagnosis_rollup
done

for host in 1 3 ; do
  ./bin/secret_share_csv pilot/secret_shares/input/$host/phame_diagnosis.csv pilot/secret_shares/input/$host/phame_diagnosis.schema pilot/secret_shares/output/$host/phame_diagnosis
  ./bin/secret_share_csv pilot/secret_shares/input/$host/phame_demographic.csv pilot/secret_shares/input/$host/phame_demographic.schema pilot/secret_shares/output/$host/phame_demographic
done