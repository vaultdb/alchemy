\echo 'Loading demographics'
\set ECHO all

-- STUDY_ID,AGE_2018,SEX,ETHINICITY,RACE,ZIPPREFIX,NUMERATOR
-- numerator missing in alliance demo
DROP TABLE IF EXISTS debug_demographics;
CREATE TABLE debug_demographics (
   pat_id int,
   study_id varchar,
   site_id varchar(2) DEFAULT 'AC',
   age_years_2015 integer, -- convert to age_days when we join with measurements
   sex varchar, 
   ethnicity varchar, 
   race varchar, -- can cast if we make all non-int cases '00'
   zipcode  varchar); 

   
\copy debug_demographics(study_id, zipcode, age_years_2015, sex, ethnicity, race) FROM   'pilot/input/alliance/demographics.csv'  CSV HEADER


-- map pid to int by removing vaultdb prefix
UPDATE debug_demographics SET study_id=REPLACE(study_id, 'VAULTDB','');
UPDATE debug_demographics SET pat_id=study_id::INT;
ALTER TABLE debug_demographics DROP COLUMN study_id;

-- get rid of NULLs since we haven't implemented them yet
-- ALTER TABLE demographics ADD COLUMN zip_marker varchar(3);
-- UPDATE demographics SET zip_marker=SUBSTRING(zipcode, 1, 3);
ALTER TABLE debug_demographics DROP COLUMN zipcode;

\echo 'Loading population labels...'

DROP TABLE IF EXISTS debug_population_labels;

CREATE TABLE debug_population_labels(
                                  pat_id INT,
                                  measure_handle varchar,
                                  study_id varchar,
                                  site_id varchar(2) default 'AC',
                                  study_year smallint,
                                  denom bool,
                                  denom_excl bool,
                                  denom_except bool, -- not sure about provenance of this columns
                                  numerator bool);

-- 113393 rows
\copy debug_population_labels(measure_handle, study_year, study_id, denom, denom_excl, numerator) FROM 'pilot/input/alliance/population-labels-1.csv' CSV HEADER;

-- 110810 rows
\copy debug_population_labels(measure_handle, study_year, study_id, denom, denom_excl, denom_except, numerator) FROM 'pilot/input/alliance/population-labels-2.csv' CSV HEADER;



\echo 'Cleaning...'




-- map pid to int by removing vaultdb prefix
UPDATE debug_population_labels SET study_id=REPLACE(study_id, 'VAULTDB','');
UPDATE debug_population_labels SET pat_id=study_id::INT;
-- drop old version of patient id
ALTER TABLE debug_population_labels DROP COLUMN study_id;


-- designed to fail if this is erroneously added to prod DB
ALTER TABLE debug_demographics RENAME TO demographics;
ALTER TABLE debug_population_labels RENAME TO population_labels;

CREATE VIEW joined_labels AS (
SELECT d.pat_id, p.study_year, d.site_id, d.age_years_2015, d.sex, d.ethnicity, d.race,  p.study_year, p.denom, p.numerator, p.denom_excl, p.denom_except
FROM demographics d JOIN population_labels p ON d.pat_id = p.pat_id  AND d.site_id = p.site_id);