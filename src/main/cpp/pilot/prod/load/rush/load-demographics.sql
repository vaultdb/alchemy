\echo 'Loading demographics'
\set echo all
-- STUDY_ID,AGE_2018,SEX,ETHINICITY,RACE,ZIPPREFIX,NUMERATOR

DROP TABLE IF EXISTS demographics;
CREATE TABLE demographics (
   study_id varchar,
   site_id varchar(2),
   age_days integer,
   sex varchar(2),
   ethnicity varchar(2), 
   race varchar(2), -- can cast if we make all non-int cases '00'
   zip_marker  varchar(3),
   numerator bool default null);
   
\copy demographics(study_id, age_days, sex, ethnicity, race, zip_marker, numerator) FROM   'pilot/input/rush/rumc_demographics_baseline_2018.csv'  CSV HEADER
ALTER TABLE demographics ADD COLUMN study_year INT;
UPDATE demographics SET study_year=2018;


\copy demographics(study_id, age_days, sex, ethnicity, race, zip_marker, numerator) FROM 'pilot/input/rush/rumc_demographics_baseline_2019.csv' CSV HEADER
UPDATE demographics SET study_year=2019 WHERE study_year IS NULL;

\copy demographics(study_id, age_days, sex, ethnicity, race, zip_marker, numerator) FROM 'pilot/input/rush/rumc_demographics_baseline_2020.csv' CSV HEADER
UPDATE demographics SET study_year=2020 WHERE study_year IS NULL;

-- map pid to int by removing vaultdb prefix
UPDATE demographics SET study_id=REPLACE(study_id, 'VAULTDB','');
ALTER TABLE demographics ADD COLUMN pat_id INT;
UPDATE demographics SET pat_id=study_id::INT;
ALTER TABLE demographics DROP COLUMN study_id;


UPDATE demographics SET site_id='RU';

-- add site ID
-- get rid of NULLs since we haven't implemented them yet
UPDATE demographics SET zip_marker='000' WHERE zip_marker IS NULL;
 
-- ethnicity:
-- Yes/No/R (refused)/NI (no info)/UN (unknown)/OT (other)
-- some have conflicting info on this (R --> N) (NI --> N), OT --> other
-- replace refused and NI values with defined ones where possible
UPDATE demographics d1 SET ethnicity=d2.ethnicity FROM demographics d2  WHERE d1.pat_id = d2.pat_id 
       		       	   			       		    	AND (d1.ethnicity = 'R' OR d1.ethnicity = 'NI') -- existing ethnicity value is undetermined
       		       	   			       		    	AND (d2.ethnicity = 'Y' OR d2.ethnicity = 'N' OR d2.ethnicity = 'OT'); -- matching one is Y/N

-- race: (from PCORI CDM)
-- 01=American Indian or Alaska Native
-- 02=Asian
-- 03=Black or African American
-- 04=Native Hawaiian or Other Pacific Islander
-- 05=White
-- 06=Multiple race
-- 07=Refuse to answer
-- NI=No information
-- UN=Unknown
-- OT=Other

-- similar issue with inconsistencies in race.
-- want to replace 07, NI, UN with well-defined vals where possible
UPDATE demographics d1 SET race=d2.race FROM demographics d2  WHERE d1.pat_id = d2.pat_id  AND d1.site_id = d2.site_id
       		       	   			       		    	AND (d1.race IN ('07', 'NI', 'UN')) -- existing race value is undetermined
       		       	   			       		    	AND (d2.race IN ('01', '02', '03', '04', '05', '06', 'OT')); -- matching one is defined above

-- if we have both a NI and a "refuse" (07)  for a patient, pick refuse so we don't double count them.
-- UPDATE demographics d1 SET race=d2.race FROM demographics d2  WHERE d1.pat_id = d2.pat_id  AND d1.site_id = d2.site_id
--        		       	   		     		            AND d1.race='NI' AND d2.race='07';


-- expected # of rows: 
-- From: SELECT COUNT(*) FROM (SELECT DISTINCT pat_id, study_year, site_id FROM demographics) t;
-- observed in this tmp table:
-- SELECT COUNT(*) FROM tmp;

DROP TABLE IF EXISTS tmp;
SELECT DISTINCT d1.pat_id, d1.study_year, d1.site_id, d1.zip_marker, d1.age_days, d1.sex, d1.ethnicity, d1.race,  COALESCE(d1.numerator, d2.numerator) numerator
INTO tmp
FROM demographics d1 LEFT JOIN demographics d2 ON d1.pat_id = d2.pat_id
                                                      AND d1.study_year = d2.study_year
                                                      AND d1.site_id = d2.site_id
                                                      AND d1.zip_marker = d2.zip_marker
                                                      AND d1.age_days = d2.age_days
                                                      AND d1.sex = d2.sex
                                                      AND d1.ethnicity = d2.ethnicity
                                                      AND d1.race = d2.race
                                                      AND d2.numerator IS NOT NULL;


-- pretty good.  Let's see what's left re: outliers:
--  SELECT * FROM tmp WHERE (pat_id, study_year, site_id)  IN (SELECT DISTINCT pat_id, study_year, site_id FROM tmp GROUP BY pat_id, study_year, site_id HAVING COUNT(*) > 1) ORDER BY pat_id;
-- for demographics:
-- WITH distinct_demo AS (
--     SELECT DISTINCT *
--     FROM demographics)
-- SELECT *
-- FROM distinct_demo
-- WHERE (pat_id, study_year, site_id) IN (
--     SELECT pat_id, study_year, site_id
--     FROM distinct_demo
--     GROUP BY pat_id, study_year, site_id
--     HAVING COUNT(*) > 1)
-- ORDER BY pat_id;

-- mostly complexity with the race or ethnicity field.  Thee are sometimes direct contradictions like a patient being listed as black or white - perhaps their entry should read 06 ( multiple race)?  Don't have enough info for these, leave them alone.
--
DROP TABLE IF EXISTS demographics;
ALTER TABLE tmp RENAME TO demographics;

UPDATE demographics SET numerator=false where numerator IS NULL;
