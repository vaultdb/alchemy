-- stitch it all together into a single DB for validation

DROP TABLE IF EXISTS patient;
CREATE TABLE patient (
    pat_id int,
    study_year int,
    age_strata char(1),
    sex  char(1),
    ethnicity  char(1), 
    race  char(1), 
    numerator bool default null,
    denom_excl bool default null,
    site_id int);

\copy patient FROM 'pilot/test/input/alice-patient.csv' CSV
\copy patient FROM 'pilot/test/input/bob-patient.csv' CSV
\copy patient FROM 'pilot/test/input/chi-patient.csv' CSV

ALTER TABLE patient ADD COLUMN multisite bool DEFAULT FALSE; 
       

UPDATE patient p1
SET multisite=true
WHERE EXISTS(
    SELECT *
    FROM patient p2
    WHERE p1.pat_id = p2.pat_id AND p1.study_year = p2.study_year AND p1.site_id <> p2.site_id);




\i  'pilot/prod/load/generate-demographics-domain.sql'
