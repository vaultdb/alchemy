-- to be run after load-demographics-nm.sql and load-population-labels-nm.sql  

\echo 'Setting up patient table!'
DROP TABLE IF EXISTS patient;


SELECT d.pat_id, d.study_year, age_stratum age_strata,
       sex, ethnicity, race,
       d.numerator AND not denom_excl numerator,
       NOT (d.numerator AND NOT p.numerator)  AND NOT denom_excl denominator,
       denom_excl, 2 as site_id
INTO patient
FROM demographics d JOIN population_labels p  ON d.pat_id = p.pat_id AND d.site_id = p.site_id AND d.study_year = p.study_year;




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


