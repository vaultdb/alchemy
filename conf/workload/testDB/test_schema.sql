-- Test DB configuration
\set site1 4
\set site2 7
\set test_year 2006

-- Create data tables
CREATE TABLE demographics (
       patient_id integer,
       birth_year integer,
       gender integer,
       race integer,
       ethnicity integer,
       insurance integer,
       zip integer);

CREATE TABLE remote_demographics (
       patient_id integer,
       birth_year integer,
       gender integer,
       race integer,
       ethnicity integer,
       insurance integer,
       zip integer);
 


CREATE TABLE diagnoses (
    patient_id integer NOT NULL,
    site integer NOT NULL,
    year integer NOT NULL,
    month integer NOT NULL,
    visit_no integer NOT NULL,
    type_ integer NOT NULL,
    encounter_id integer NOT NULL,
    diag_src character varying NOT NULL,
    icd9 character varying NOT NULL,
    primary_ integer NOT NULL,
    timestamp_ timestamp without time zone,
    clean_icd9 character varying,
    major_icd9 character varying
);

CREATE TABLE remote_diagnoses (
    patient_id integer NOT NULL,
    site integer NOT NULL,
    year integer NOT NULL,
    month integer NOT NULL,
    visit_no integer NOT NULL,
    type_ integer NOT NULL,
    encounter_id integer NOT NULL,
    diag_src character varying NOT NULL,
    icd9 character varying NOT NULL,
    primary_ integer NOT NULL,
    timestamp_ timestamp without time zone,
    clean_icd9 character varying,
    major_icd9 character varying
);


DROP TABLE IF EXISTS cdiff_cohort;
CREATE TABLE cdiff_cohort AS (
       SELECT DISTINCT patient_id FROM diagnoses WHERE icd9 = '008.45' AND year = :test_year AND (site=:site1 OR site=:site2));

DROP TABLE IF EXISTS mi_cohort;
CREATE TABLE mi_cohort AS (
       SELECT DISTINCT patient_id FROM diagnoses WHERE icd9 LIKE '410%'  AND year = :test_year AND (site=:site1 OR site=:site2));



 -- data used in test to simulate patient registry
DROP TABLE IF EXISTS cdiff_cohort_diagnoses;
CREATE TABLE cdiff_cohort_diagnoses AS SELECT * FROM diagnoses WHERE year = :test_year AND (site=:site1 OR site=:site2) AND patient_id IN (SELECT * FROM cdiff_cohort); 
DROP TABLE IF EXISTS mi_cohort_diagnoses;
CREATE TABLE mi_cohort_diagnoses AS SELECT * FROM diagnoses  WHERE year = :test_year AND (site=:site1 OR site=:site2) AND patient_id IN (SELECT * FROM mi_cohort); 

-- 

CREATE TABLE vitals (
    patient_id integer NOT NULL,
    site integer NOT NULL,
    year integer NOT NULL,
    month integer NOT NULL,
    visit_no integer NOT NULL,
    height numeric,
    height_units character varying,
    weight numeric,
    weight_units character varying,
    bmi numeric,
    bmi_units character varying,
    systolic numeric,
    diastolic numeric,
    pulse numeric,
    bp_method character varying,
    timestamp_ timestamp without time zone
);    

DROP TABLE IF EXISTS mi_cohort_vitals;
CREATE TABLE mi_cohort_vitals AS SELECT * FROM vitals WHERE year = :test_year AND (site=:site1 OR site=:site2) AND patient_id IN (SELECT * FROM mi_cohort); 

CREATE TABLE labs (
    patient_id integer,
    timestamp_ timestamp,
    test_name character varying,
    value_ character varying,
    unit character varying,
    value_low real,
    value_high real);


CREATE TABLE medications (
    patient_id integer NOT NULL,
    site integer NOT NULL,
    year integer NOT NULL,
    month integer NOT NULL,
    medication character varying NOT NULL,
    dosage character varying NOT NULL,
    route character varying,
    timestamp_ timestamp without time zone
);


CREATE TABLE remote_medications (
    patient_id integer NOT NULL,
    site integer NOT NULL,
    year integer NOT NULL,
    month integer NOT NULL,
    medication character varying NOT NULL,
    dosage character varying NOT NULL,
    route character varying,
    timestamp_ timestamp without time zone
);

CREATE TABLE site (
       id integer);



CREATE TABLE remote_medications (
    patient_id integer NOT NULL,
    site integer NOT NULL,
    year integer NOT NULL,
    month integer NOT NULL,
    medication character varying NOT NULL,
    dosage character varying NOT NULL,
    route character varying,
    timestamp_ timestamp without time zone
);

DROP TABLE IF EXISTS mi_cohort_medications;
CREATE TABLE mi_cohort_medications AS SELECT * FROM medications  WHERE year = :test_year AND (site=:site1 OR site=:site2) AND patient_id IN (SELECT * FROM mi_cohort); 

CREATE TABLE a_diagnoses (
    patient_id integer,
    icd9 character varying,
    timestamp_ timestamp);

CREATE TABLE a_init (
    lenD integer,
    lenM integer,
    pad timestamp);

CREATE TABLE test (
    privc varchar(1),
    privi integer);


-- set up security level of attributes
-- default setting: attribute is private
CREATE ROLE public_attribute;
CREATE ROLE protected_attribute;
GRANT SELECT(patient_id) ON diagnoses TO public_attribute;
GRANT SELECT(visit_no) ON diagnoses TO public_attribute;
GRANT SELECT(primary_) ON diagnoses TO public_attribute;
GRANT SELECT(diag_src) ON diagnoses TO protected_attribute;
GRANT SELECT(icd9) ON diagnoses TO protected_attribute;
GRANT SELECT(major_icd9) ON diagnoses TO protected_attribute;

GRANT SELECT(patient_id) ON cdiff_cohort_diagnoses TO public_attribute;
GRANT SELECT(visit_no) ON cdiff_cohort_diagnoses TO public_attribute;
GRANT SELECT(primary_) ON cdiff_cohort_diagnoses TO public_attribute;
GRANT SELECT(diag_src) ON cdiff_cohort_diagnoses TO protected_attribute;
GRANT SELECT(icd9) ON cdiff_cohort_diagnoses TO protected_attribute;
GRANT SELECT(major_icd9) ON cdiff_cohort_diagnoses TO protected_attribute;

GRANT SELECT(patient_id) ON mi_cohort_diagnoses TO public_attribute;
GRANT SELECT(visit_no) ON mi_cohort_diagnoses TO public_attribute;
GRANT SELECT(primary_) ON mi_cohort_diagnoses TO public_attribute;
GRANT SELECT(diag_src) ON mi_cohort_diagnoses TO protected_attribute;
GRANT SELECT(icd9) ON mi_cohort_diagnoses TO protected_attribute;
GRANT SELECT(major_icd9) ON mi_cohort_diagnoses TO protected_attribute;

GRANT SELECT(patient_id) ON medications TO public_attribute;
GRANT SELECT(dosage) ON medications TO public_attribute;
GRANT SELECT(route) ON medications TO public_attribute;
GRANT SELECT(medication) ON medications TO protected_attribute;

GRANT SELECT(patient_id) ON mi_cohort_medications TO public_attribute;
GRANT SELECT(dosage) ON mi_cohort_medications TO public_attribute;
GRANT SELECT(route) ON mi_cohort_medications TO public_attribute;
GRANT SELECT(medication) ON mi_cohort_medications TO protected_attribute;

GRANT SELECT(patient_id) ON vitals TO public_attribute;
GRANT SELECT(visit_no) ON vitals TO public_attribute;
GRANT SELECT(height_units) ON vitals TO public_attribute;
GRANT SELECT(weight_units) ON vitals TO public_attribute;
GRANT SELECT(bp_method) ON vitals TO public_attribute;
GRANT SELECT(height) ON vitals TO protected_attribute;
GRANT SELECT(weight) ON vitals TO protected_attribute;
GRANT SELECT(bmi) ON vitals TO protected_attribute;
GRANT SELECT(systolic) ON vitals TO protected_attribute;
GRANT SELECT(diastolic) ON vitals TO protected_attribute;
GRANT SELECT(pulse) ON vitals TO protected_attribute;

GRANT SELECT(patient_id) ON mi_cohort_vitals TO public_attribute;
GRANT SELECT(visit_no) ON mi_cohort_vitals TO public_attribute;
GRANT SELECT(height_units) ON mi_cohort_vitals TO public_attribute;
GRANT SELECT(weight_units) ON mi_cohort_vitals TO public_attribute;
GRANT SELECT(bp_method) ON mi_cohort_vitals TO public_attribute;
GRANT SELECT(height) ON mi_cohort_vitals TO protected_attribute;
GRANT SELECT(weight) ON mi_cohort_vitals TO protected_attribute;
GRANT SELECT(bmi) ON mi_cohort_vitals TO protected_attribute;
GRANT SELECT(systolic) ON mi_cohort_vitals TO protected_attribute;
GRANT SELECT(diastolic) ON mi_cohort_vitals TO protected_attribute;
GRANT SELECT(pulse) ON mi_cohort_vitals TO protected_attribute;

GRANT SELECT(patient_id) ON demographics TO public_attribute;
GRANT SELECT(gender) ON demographics TO protected_attribute;
GRANT SELECT(race) ON demographics TO protected_attribute;
GRANT SELECT(ethnicity) ON demographics TO protected_attribute;
GRANT SELECT(insurance) ON demographics TO protected_attribute;
GRANT SELECT(birth_year) ON demographics TO protected_attribute;

-- constraints for cardinality bounds
ALTER TABLE demographics ADD CONSTRAINT  year_range CHECK  (birth_year >= 1900 AND birth_year <= date_part('year', CURRENT_DATE));
ALTER TABLE demographics ADD CONSTRAINT  gender_range CHECK  (gender >= 1 AND gender <= 3);  
ALTER TABLE demographics ADD CONSTRAINT  race_range CHECK  (race >= 1 AND race <= 8);  
ALTER TABLE demographics ADD CONSTRAINT  ethnicity_range CHECK  (ethnicity >= 1 AND ethnicity <= 2);  
ALTER TABLE demographics ADD CONSTRAINT  insurance_range CHECK  (insurance >= 1 AND insurance <= 6);  
ALTER TABLE diagnoses ADD CONSTRAINT month_range CHECK (month >= 1 AND month <= 12);

-- stats table for cardinality bounds
CREATE TABLE relation_statistics(relation varchar, attr varchar, distinct_values integer, max_multiplicity integer, min_value integer, max_value integer); 