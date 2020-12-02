-- mock-up of the logic of HTN protocol
-- starting from enrich_htn_alice


\c enrich_htn_alice

DROP TABLE IF EXISTS patient;
CREATE TABLE patient (
    patid int,
    zip_marker varchar(3),
    age_days integer,
    sex varchar(1),
    ethnicity int, -- 0 || 1
    race int,
    numerator int default null -- denotes null = false, 1 = true
);

DROP TABLE IF EXISTS  patient_exclusion;
CREATE TABLE patient_exclusion (
    patid int,
    numerator int default  null,
    denom_excl int default null
);


\copy patient FROM 'alice-patient.csv' CSV
\copy patient_exclusion FROM 'alice-patient-exclusion.csv' CSV


\c enrich_htn_bob

DROP TABLE IF EXISTS patient;
CREATE TABLE patient (
    patid int,
    zip_marker varchar(3),
    age_days integer,
    sex varchar(1),
    ethnicity int, -- 0 || 1
    race int,
    numerator int default null -- denotes null = false, 1 = true
);

DROP TABLE IF EXISTS  patient_exclusion;
CREATE TABLE patient_exclusion (
    patid int,
    numerator int default  null,
    denom_excl int default null
);

\copy patient FROM 'bob-patient.csv' CSV
\copy patient_exclusion FROM 'bob-patient-exclusion.csv' CSV

-- stitch it all together into a single DB for validation
\c enrich_htn_unioned
DROP TABLE IF EXISTS patient;
CREATE TABLE patient (
    patid int,
    zip_marker varchar(3),
    age_days integer,
    sex varchar(1),
    ethnicity int, -- 0 || 1
    race int,
    numerator int default null -- denotes null = false, 1 = true
);

\copy patient FROM 'alice-patient.csv' CSV
ALTER TABLE patient ADD COLUMN site_id INT;
UPDATE patient SET site_id = 1;

\copy patient(patid, zip_marker, age_days, sex, ethnicity, race, numerator) FROM 'bob-patient.csv' CSV
UPDATE patient SET site_id = 2 WHERE site_id IS NULL;


DROP TABLE IF EXISTS  patient_exclusion;
CREATE TABLE patient_exclusion (
    patid int,
    numerator int default  null,
    denom_excl int default null
);


\copy patient_exclusion FROM 'alice-patient-exclusion.csv' CSV
ALTER TABLE patient_exclusion ADD COLUMN site_id INT;
UPDATE patient_exclusion SET site_id = 1;

\copy patient_exclusion(patid,  numerator, denom_excl) FROM 'bob-patient-exclusion.csv' CSV
UPDATE patient_exclusion SET site_id = 2 WHERE site_id IS NULL;




