#!/bin/bash

# Extract cardinality from JSON
CARD=$(grep -m1 '"cardinality"' conf/datagen/healthlnk.json | sed -E 's/[^0-9]*([0-9]+).*/\1/')

DB_UNIONED="healthlnk_unioned_${CARD}"
DB_ALICE="healthlnk_alice_${CARD}"
DB_BOB="healthlnk_bob_${CARD}"

make generate_synthetic_data
./bin/generate_synthetic_data conf/datagen/healthlnk.json


# Drop existing databases
dropdb $DB_UNIONED
dropdb $DB_ALICE
dropdb $DB_BOB

# Create databases
createdb $DB_UNIONED
createdb $DB_ALICE
createdb $DB_BOB

# Unioned setup
psql $DB_UNIONED -c "CREATE TABLE diagnoses(patient_id int, year_ int, timestamp_ bigint, visit_no int, type_ int, encounter_id int, diag_src varchar(32), icd9 varchar(7), major_icd9 varchar(4), primary_ int, party int)"
psql $DB_UNIONED -c "CREATE TABLE medications(patient_id int, year_ int, timestamp_ bigint, medication varchar(10), dosage varchar(10), route varchar(14), party int)"

# Load party 0
psql $DB_UNIONED -c "\copy diagnoses(patient_id, year_, timestamp_, visit_no, type_, encounter_id, diag_src, icd9, major_icd9, primary_) FROM 'conf/workload/healthlnk/0/diagnoses.csv' DELIMITER ','"
psql $DB_UNIONED -c "UPDATE diagnoses SET party=0 WHERE party IS NULL"
psql $DB_UNIONED -c "\copy medications(patient_id, year_, timestamp_, medication, dosage, route) FROM 'conf/workload/healthlnk/0/medications.csv' DELIMITER ','"
psql $DB_UNIONED -c "UPDATE medications SET party=0 WHERE party IS NULL"

# Load party 1
psql $DB_UNIONED -c "\copy diagnoses(patient_id, year_, timestamp_, visit_no, type_, encounter_id, diag_src, icd9, major_icd9, primary_) FROM 'conf/workload/healthlnk/1/diagnoses.csv' DELIMITER ','"
psql $DB_UNIONED -c "UPDATE diagnoses SET party=1 WHERE party IS NULL"
psql $DB_UNIONED -c "\copy medications(patient_id, year_, timestamp_, medication, dosage, route) FROM 'conf/workload/healthlnk/1/medications.csv' DELIMITER ','"
psql $DB_UNIONED -c "UPDATE medications SET party=1 WHERE party IS NULL"

# Trim string fields
psql $DB_UNIONED -c "UPDATE diagnoses SET icd9 = TRIM(icd9), major_icd9 = TRIM(major_icd9);"
psql $DB_UNIONED -c "UPDATE medications SET medication = TRIM(medication);"

# Bit packing + pids table
psql $DB_UNIONED -c "CREATE TABLE bit_packing ( table_name VARCHAR(255), col_name VARCHAR(255), min INT, max INT, domain_size INT );"
psql $DB_UNIONED -c "CREATE TABLE IF NOT EXISTS pids_in_mpc ( patient_id INT );"

# Alice setup
psql $DB_ALICE -c "CREATE TABLE diagnoses(patient_id int, year_ int, timestamp_ bigint, visit_no int, type_ int, encounter_id int, diag_src varchar(32), icd9 varchar(7), major_icd9 varchar(4), primary_ int)"
psql $DB_ALICE -c "CREATE TABLE medications(patient_id int, year_ int, timestamp_ bigint, medication varchar(10), dosage varchar(10), route varchar(14))"
psql $DB_ALICE -c "\copy diagnoses FROM 'conf/workload/healthlnk/0/diagnoses.csv' DELIMITER ','"
psql $DB_ALICE -c "\copy medications FROM 'conf/workload/healthlnk/0/medications.csv' DELIMITER ','"
psql $DB_ALICE -c "UPDATE diagnoses SET icd9 = TRIM(icd9), major_icd9 = TRIM(major_icd9);"
psql $DB_ALICE -c "UPDATE medications SET medication = TRIM(medication);"
psql $DB_ALICE -c "CREATE TABLE bit_packing ( table_name VARCHAR(255), col_name VARCHAR(255), min INT, max INT, domain_size INT );"
psql $DB_ALICE -c "CREATE TABLE IF NOT EXISTS pids_in_mpc_public_join ( patient_id INT, diagnosis_timestamp INT, icd9 varchar(7), medication_patient_id INT, medication_timestamp INT, medication varchar(10) );"
psql $DB_ALICE -c "CREATE TABLE IF NOT EXISTS pids_in_mpc ( patient_id INT );"

# Bob setup
psql $DB_BOB -c "CREATE TABLE diagnoses(patient_id int, year_ int, timestamp_ bigint, visit_no int, type_ int, encounter_id int, diag_src varchar(32), icd9 varchar(7), major_icd9 varchar(4), primary_ int)"
psql $DB_BOB -c "CREATE TABLE medications(patient_id int, year_ int, timestamp_ bigint, medication varchar(10), dosage varchar(10), route varchar(14))"
psql $DB_BOB -c "\copy diagnoses FROM 'conf/workload/healthlnk/1/diagnoses.csv' DELIMITER ','"
psql $DB_BOB -c "\copy medications FROM 'conf/workload/healthlnk/1/medications.csv' DELIMITER ','"
psql $DB_BOB -c "UPDATE diagnoses SET icd9 = TRIM(icd9), major_icd9 = TRIM(major_icd9);"
psql $DB_BOB -c "UPDATE medications SET medication = TRIM(medication);"
psql $DB_BOB -c "CREATE TABLE bit_packing ( table_name VARCHAR(255), col_name VARCHAR(255), min INT, max INT, domain_size INT );"
psql $DB_BOB -c "CREATE TABLE IF NOT EXISTS pids_in_mpc_public_join ( patient_id INT, diagnosis_timestamp INT, icd9 varchar(7), medication_patient_id INT, medication_timestamp INT, medication varchar(10) );"
psql $DB_BOB -c "CREATE TABLE IF NOT EXISTS pids_in_mpc ( patient_id INT );"

# Setup and join queries
TMP_JOIN_CSV="/tmp/pids_in_mpc_public_join_${CARD}.csv"
TMP_PID_CSV="/tmp/pids_in_mpc_healthlnk_${CARD}.csv"

psql $DB_UNIONED -c "\copy (WITH a_matches AS (
    SELECT d.patient_id FROM diagnoses d JOIN medications m ON m.patient_id = d.patient_id WHERE d.party = 0 AND m.party = 1
), b_matches AS (
    SELECT d.patient_id FROM diagnoses d JOIN medications m ON m.patient_id = d.patient_id WHERE d.party = 1 AND m.party = 0
), pids_in_mpc AS (
    SELECT * FROM a_matches UNION SELECT * FROM b_matches
)
SELECT d.patient_id, d.timestamp_ AS diagnosis_timestamp, d.icd9, m.patient_id AS medication_patient_id, m.timestamp_ AS medication_timestamp, m.medication
FROM diagnoses d JOIN medications m ON d.patient_id = m.patient_id
WHERE d.patient_id IN (SELECT patient_id FROM pids_in_mpc)
  AND m.patient_id IN (SELECT patient_id FROM pids_in_mpc)
ORDER BY d.patient_id) TO '${TMP_JOIN_CSV}' WITH CSV HEADER;"

psql $DB_ALICE -c "\copy pids_in_mpc_public_join FROM '${TMP_JOIN_CSV}' WITH CSV HEADER;"

psql $DB_UNIONED -c "\copy (WITH a_matches AS (
    SELECT d.patient_id FROM diagnoses d JOIN medications m ON m.patient_id = d.patient_id WHERE d.party = 0 AND m.party = 1
), b_matches AS (
    SELECT d.patient_id FROM diagnoses d JOIN medications m ON m.patient_id = d.patient_id WHERE d.party = 1 AND m.party = 0
), pids_in_mpc AS (
    SELECT * FROM a_matches UNION SELECT * FROM b_matches
)
SELECT * FROM pids_in_mpc ORDER BY patient_id) TO '${TMP_PID_CSV}' WITH CSV HEADER;"

psql $DB_ALICE -c "\copy pids_in_mpc FROM '${TMP_PID_CSV}' WITH CSV HEADER;"
psql $DB_BOB -c "\copy pids_in_mpc FROM '${TMP_PID_CSV}' WITH CSV HEADER;"
