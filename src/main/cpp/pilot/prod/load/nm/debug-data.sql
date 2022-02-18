\echo 'Loading demographics'
\set ECHO all
-- STUDY_ID,SITE_ID,AGE_2018,SEX,ETHINICITY,RACE,ZIPPREFIX,NUMERATOR

DROP TABLE IF EXISTS debug_demographics;
CREATE TABLE debug_demographics (
   study_id varchar,
   site_id varchar(2),
   age_days integer,
   sex varchar(2),
   ethnicity varchar(2), 
   race varchar(2), -- can cast if we make all non-int cases '00'
   zip_marker  varchar(3),
   numerator bool default null);
   
\copy demographics FROM 'pilot/input/nm/VaultDB_DEMOGRAPHIC_BASELINE_2018.csv' CSV HEADER
ALTER TABLE debug_demographics ADD COLUMN study_year SMALLINT;
UPDATE debug_demographics SET study_year=2018;


\copy debug_demographics(study_id, site_id, age_days, sex, ethnicity, race, zip_marker, numerator) FROM 'pilot/input/nm/VaultDB_DEMOGRAPHIC_BASELINE_2019.csv' CSV HEADER
UPDATE debug_demographics SET study_year=2019 WHERE study_year IS NULL;

\copy debug_demographics(study_id, site_id, age_days, sex, ethnicity, race, zip_marker, numerator) FROM 'pilot/input/nm/VaultDB_DEMOGRAPHIC_BASELINE_2020.csv' CSV HEADER
UPDATE debug_demographics SET study_year=2020 WHERE study_year IS NULL;

-- map pid to int by removing vaultdb prefix
UPDATE debug_demographics SET study_id=REPLACE(study_id, 'VAULTDB','');
ALTER TABLE debug_demographics ADD COLUMN pat_id INT;
UPDATE debug_demographics SET pat_id=study_id::INT;
ALTER TABLE debug_demographics DROP COLUMN study_id;

-- all zips are empty in this data
ALTER TABLE debug_demographics DROP COLUMN zip_marker;

ALTER TABLE debug_demographics ADD COLUMN age_strata char(1);

UPDATE debug_demographics SET age_strata = CASE WHEN age_days <= 28*365 THEN '1'
                WHEN age_days > 28*365 AND age_days <= 39*365 THEN '2'
              WHEN age_days > 39*365  AND age_days <= 50*365 THEN '3'
              WHEN age_days > 50*365 AND age_days <= 61*365 THEN '4'
              WHEN age_days > 61*365 AND age_days <= 72*365 THEN '5'
                WHEN age_days > 72*365  AND age_days <= 83*365 THEN '6'
                ELSE '7' END;

ALTER TABLE debug_demographics DROP COLUMN age_days;


UPDATE debug_demographics SET sex='U' WHERE sex='UN';
ALTER TABLE debug_demographics ALTER COLUMN sex TYPE char(1);



\echo 'Loading population labels'
DROP TABLE IF EXISTS debug_population_labels;

-- setup:
-- STUDY_ID,SITE_ID,NUMERATOR,DENOM_EXCL


-- before this, in the data directory (pilot/input/nm/ here) run:
--  for f in $(ls *.csv); do echo "processing $f" ; sed -i 's/,NULL/,/g' $f; done

-- For study, we basically have 3 groups:
-- numerator: study population (demographics.numerator = true)
-- denominator: baseline population - everyone not in denom_excl or where population_labels.numerator = false
-- denom_excl: folks who are excluded from the study entirely.
-- population_labels.numerator = true if patient in both numerator and denom groups

CREATE TABLE debug_population_labels (
       pat_id INT,
      study_id varchar,
      site_id varchar(2),
      numerator bool,
      denom_excl bool);

\copy debug_population_labels(study_id, site_id, numerator, denom_excl)  FROM 'pilot/input/nm/VaultDB_POPULATION_LABELS_2018.csv' CSV HEADER
ALTER TABLE debug_population_labels ADD COLUMN study_year SMALLINT;
UPDATE debug_population_labels SET study_year=2018;

\copy debug_population_labels(study_id, site_id, numerator, denom_excl) FROM 'pilot/input/nm/VaultDB_POPULATION_LABELS_2019.csv' CSV HEADER
UPDATE debug_population_labels SET study_year=2019 WHERE study_year IS NULL;

\copy debug_population_labels(study_id, site_id, numerator, denom_excl) FROM 'pilot/input/nm/VaultDB_POPULATION_LABELS_2020.csv' CSV HEADER
UPDATE debug_population_labels SET study_year=2020 WHERE study_year IS NULL;

-- map pid to int by removing vaultdb prefix
UPDATE debug_population_labels SET study_id=REPLACE(study_id, 'VAULTDB','');
UPDATE debug_population_labels SET pat_id=study_id::INT;
-- drop old version of patient id
ALTER TABLE debug_population_labels DROP COLUMN study_id;

