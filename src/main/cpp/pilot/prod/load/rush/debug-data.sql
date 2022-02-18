\echo 'Loading demographics'
\set ECHO all
-- STUDY_ID,AGE_2018,SEX,ETHINICITY,RACE,ZIPPREFIX,NUMERATOR

DROP TABLE IF EXISTS debug_demographics;
CREATE TABLE debug_demographics (
       pat_id INT,
       study_id varchar,
   site_id varchar(2),
   age_days integer,
sex varchar(2),
   ethnicity varchar(2), 
   race varchar(2), -- can cast if we make all non-int cases '00'
   zip_marker  varchar(3),
   numerator bool default null);
   
\copy debug_demographics(study_id, age_days, sex, ethnicity, race, zip_marker, numerator) FROM   'pilot/input/rush/rumc_demographics_baseline_2018.csv'  CSV HEADER
ALTER TABLE debug_demographics ADD COLUMN study_year INT;
UPDATE debug_demographics SET study_year=2018;


\copy debug_demographics(study_id, age_days, sex, ethnicity, race, zip_marker, numerator) FROM 'pilot/input/rush/rumc_demographics_baseline_2019.csv' CSV HEADER
UPDATE debug_demographics SET study_year=2019 WHERE study_year IS NULL;

\copy debug_demographics(study_id, age_days, sex, ethnicity, race, zip_marker, numerator) FROM 'pilot/input/rush/rumc_demographics_baseline_2020.csv' CSV HEADER
UPDATE debug_demographics SET study_year=2020 WHERE study_year IS NULL;

-- map pid to int by removing vaultdb prefix
UPDATE debug_demographics SET study_id=REPLACE(study_id, 'VAULTDB','');
UPDATE debug_demographics SET pat_id=study_id::INT;
ALTER TABLE debug_demographics DROP COLUMN study_id;
ALTER TABLE debug_demographics DROP COLUMN zip_marker;

UPDATE debug_demographics SET site_id='RU';

ALTER TABLE debug_demographics ADD COLUMN age_stratum char(1);



UPDATE debug_demographics SET age_stratum = CASE WHEN age_days <= 28*365 THEN '1'
                WHEN age_days > 28*365 AND age_days <= 39*365 THEN '2'
              WHEN age_days > 39*365  AND age_days <= 50*365 THEN '3'
              WHEN age_days > 50*365 AND age_days <= 61*365 THEN '4'
              WHEN age_days > 61*365 AND age_days <= 72*365 THEN '5'
                WHEN age_days > 72*365  AND age_days <= 83*365 THEN '6'
                ELSE '7' END;




ALTER TABLE debug_demographics DROP COLUMN age_days;



--  both of these queries produce empty sets during setup
\echo 'Loading population labels'
DROP TABLE IF EXISTS debug_population_labels;


CREATE TABLE debug_population_labels (
      pat_id int, 
      study_id varchar,
      site_id varchar(2),
      numerator bool,
      denom_excl bool);

\echo 'Loading 2018'
\copy debug_population_labels(study_id, numerator, denom_excl) FROM 'pilot/input/rush/rumc_population_labels_2018.csv' CSV HEADER
ALTER TABLE debug_population_labels ADD COLUMN study_year INT;
UPDATE debug_population_labels SET study_year=2018;

\echo 'Loading 2019'
\copy debug_population_labels(study_id, numerator, denom_excl) FROM 'pilot/input/rush/rumc_population_labels_2019.csv' CSV HEADER
UPDATE debug_population_labels SET study_year=2019 WHERE study_year IS NULL;

\echo 'Loading 2020'
\copy debug_population_labels(study_id, numerator, denom_excl) FROM 'pilot/input/rush/rumc_population_labels_2020.csv' CSV HEADER
UPDATE debug_population_labels SET study_year=2020 WHERE study_year IS NULL;


\echo 'Cleaning...'

UPDATE population_labels SET site_id='RU';

-- map pid to int by removing vaultdb prefix
UPDATE population_labels SET study_id=REPLACE(study_id, 'VAULTDB','');
UPDATE population_labels SET pat_id=study_id::INT;
-- drop old version of patient id
ALTER TABLE population_labels DROP COLUMN study_id;

-- designed to fail if this is erroneously added to prod DB
ALTER TABLE debug_demographics RENAME TO demographics;
ALTER TABLE debug_population_labels RENAME TO population_labels;

CREATE VIEW joined_labels AS (
    SELECT d.study_year, d.pat_id, d.site_id, d.age_strata, d.sex, d.ethnicity, d.race, d.numerator demographics_numerator, p.numerator population_numerator, p.denom_excl
    FROM demographics d JOIN population_labels p ON d.pat_id = p.pat_id AND d.study_year = p.study_year AND d.site_id = p.site_id);

