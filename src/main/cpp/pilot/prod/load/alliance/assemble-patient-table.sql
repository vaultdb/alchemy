-- to be run after load-demographics.sql and load-population-labels.sql  

\echo 'Setting up patient table!'
DROP TABLE IF EXISTS patient;





SELECT d.pat_id, study_year, 0 as  age_days,  '0' as age_strata, -- placeholder 
       age_years_2015, sex, ethnicity, race,
       numerator AND not denom_excl numerator,
       denom AND NOT denom_excl denominator,
       denom_excl, 3 as site_id
INTO patient
FROM demographics d JOIN population_labels p  ON d.pat_id = p.pat_id AND d.site_id = p.site_id;


-- populate age_days
UPDATE patient
SET age_days = (age_years_2015 + (study_year - 2015)) * 365;




ALTER TABLE patient ALTER COLUMN age_strata TYPE char(1);

UPDATE patient SET age_strata = CASE WHEN age_days <= 28*365 THEN '1'
                WHEN age_days > 28*365 AND age_days <= 39*365 THEN '2'
              WHEN age_days > 39*365  AND age_days <= 50*365 THEN '3'
              WHEN age_days > 50*365 AND age_days <= 61*365 THEN '4'
              WHEN age_days > 61*365 AND age_days <= 72*365 THEN '5'
                WHEN age_days > 72*365  AND age_days <= 83*365 THEN '6'
                ELSE '7' END;


ALTER TABLE patient DROP COLUMN age_days;
ALTER TABLE patient DROP COLUMN age_years_2015;

ALTER TABLE patient ADD COLUMN multisite bool DEFAULT false;

CREATE TABLE multisite_pids(
pat_id INT,
study_year smallint);

-- requires offline reconciliation of pat_ids with other sites
\copy multisite_pids FROM 'pilot/output/alliance-multisite.csv' CSV HEADER

UPDATE patient p
SET multisite=true
WHERE EXISTS (
      SELECT *
      FROM multisite_pids m
      WHERE m.pat_id = p.pat_id AND m.study_year = p.study_year);

DROP TABLE multisite_pids;
\i 'pilot/prod/load/add-hashes-to-patids.sql'

-- look for dupes:
-- SELECT pat_id, study_year, site_id FROM patient GROUP  BY pat_id, study_year, site_id HAVING COUNT(*) > 1 ORDER BY pat_id;




-- at the end we have:
-- > \d patient
--                        Table "public.patient"
--    Column    |         Type         | Collation | Nullable | Default 
-- -------------+----------------------+-----------+----------+---------
--  pat_id      | integer              |           |          | 
--  study_year  | integer              |           |          | 
--  age_days    | integer              |           |          | 
--  sex         | character varying(2) |           |          | 
--  ethnicity   | character varying(2) |           |          | 
--  race        | character varying(2) |           |          | 
--  numerator   | boolean              |           |          | false
--  denom_excl  | boolean              |           |          | false
--  site_id     | integer              |           |          | 
-- 
