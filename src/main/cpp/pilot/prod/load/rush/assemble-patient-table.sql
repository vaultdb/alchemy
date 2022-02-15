-- to be run after load-demographics-nm.sql and load-population-labels-nm.sql  

\echo 'Setting up patient table!'

DROP TABLE IF EXISTS patient;


CREATE TABLE patient (
                         pat_id INT,
                         study_year smallint,
                         age_strata char(1) DEFAULT '0', -- unknown
                         sex char(1) DEFAULT 'U',
                         ethnicity char(2) DEFAULT 'U',
                         race char(2) DEFAULT '7', -- unknown
                         numerator boolean default false,
                         population_numerator bool, -- temp
                         denominator boolean default true,
                         denom_excl boolean default false,
                         site_id INT DEFAULT 2);


-- 4 possible settings for cohort:
-- '0' - exclusion
-- '1' - denominator-only (default)
-- '2' numerator-only
-- '3' both
-- if d.numerator - numerator only
-- if p.numerator - belongs to both  num and denom
INSERT INTO patient(pat_id, study_year, population_numerator, denom_excl) SELECT pat_id, study_year, numerator, denom_excl FROM population_labels;

UPDATE patient p
SET age_strata = d.age_stratum, sex = d.sex, ethnicity = d.ethnicity, race = d.race, numerator = d.numerator
FROM demographics d
WHERE d.study_year = p.study_year AND d.pat_id = p.pat_id;

-- denominator only false where d.numerator and not p.numerator
UPDATE patient SET denominator = false WHERE numerator AND NOT population_numerator;
UPDATE patient SET denominator = false, numerator=false WHERE denom_excl;

ALTER TABLE patient DROP COLUMN population_numerator;

UPDATE patient SET sex='U' WHERE sex IS NULL;
UPDATE patient SET ethnicity='U' WHERE ethnicity <> 'Y' AND ethnicity <> 'N';
UPDATE patient SET race='07' WHERE race NOT IN ('01', '02', '03', '04', '05', '06', '07');

ALTER TABLE patient ALTER COLUMN sex TYPE CHAR(1);
ALTER TABLE patient ALTER COLUMN ethnicity TYPE CHAR(1);
ALTER TABLE patient ALTER COLUMN race TYPE CHAR(1) using substring(race, 2, 1);


ALTER TABLE patient ADD COLUMN multisite bool DEFAULT false;

CREATE TABLE multisite_pids(
       pat_id INT,
       study_year smallint);

-- requires offline reconciliation of pat_ids with other sites
\copy multisite_pids FROM 'pilot/output/rush-multisite.csv' CSV HEADER

UPDATE patient p
SET multisite=true
WHERE EXISTS (
      SELECT *
      FROM multisite_pids m
      WHERE m.pat_id = p.pat_id AND m.study_year = p.study_year);

DROP TABLE multisite_pids;

\i 'pilot/prod/load/add-hashes-to-patids.sql'
-- look for dupes:
-- SELECT pat_id, study_year 
-- FROM patient 
-- GROUP  BY pat_id, study_year 
-- HAVING COUNT(*) > 1 
-- ORDER BY pat_id;


