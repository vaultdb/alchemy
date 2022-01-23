-- to be run after load-demographics-nm.sql and load-population-labels-nm.sql  


\echo 'Setting up patient table!'
DROP TABLE IF EXISTS patient;

SELECT d.pat_id, d.study_year, age_strata,
       sex, ethnicity, race,
       p.numerator AND not denom_excl numerator,
       denom_excl, 1 as site_id
INTO patient
FROM demographics d JOIN population_labels p  ON d.pat_id = p.pat_id AND d.site_id = p.site_id AND d.study_year = p.study_year;




-- SELECT * FROM patient WHERE pat_id = 208865;
-- 1 per year, extra one in  2019 = 4 rows
UPDATE patient SET ethnicity='U' WHERE pat_id = 208865;

-- deduplicate
DROP TABLE IF EXISTS tmp;
SELECT DISTINCT * INTO tmp FROM patient;
DROP TABLE patient;
ALTER TABLE tmp RENAME TO patient;


ALTER TABLE patient ADD COLUMN multisite bool DEFAULT false;

CREATE TABLE multisite_pids(
       pat_id INT,
       study_year smallint);

-- requires offline reconciliation of pat_ids with other sites
\copy multisite_pids FROM 'pilot/output/nm-multisite.csv' CSV HEADER

UPDATE patient p
SET multisite=true
WHERE EXISTS (
      SELECT *
      FROM multisite_pids m
      WHERE m.pat_id = p.pat_id AND m.study_year = p.study_year);

DROP TABLE multisite_pids;

\i 'pilot/prod/load/add-hashes-to-patids.sql'

-- look for inconsistencies:
--  SELECT pat_id, study_year, site_id FROM patient GROUP  BY pat_id, study_year, site_id HAVING COUNT(*) > 1 ORDER BY pat_id;
