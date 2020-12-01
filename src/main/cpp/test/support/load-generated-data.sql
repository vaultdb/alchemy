-- mock-up of the logic of HTN protocol
-- starting from enrich_htn_unioned

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


\copy patient FROM 'unioned-patient.csv' CSV
\copy patient_exclusion FROM 'unioned-patient-exclusion.csv' CSV
 

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
