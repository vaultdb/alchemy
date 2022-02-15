\echo 'Loading demographics'
\set ECHO all
-- STUDY_ID,SITE_ID,AGE_2018,SEX,ETHINICITY,RACE,ZIPPREFIX,NUMERATOR

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
   
\copy demographics FROM 'pilot/input/nm/VaultDB_DEMOGRAPHIC_BASELINE_2018.csv' CSV HEADER
ALTER TABLE demographics ADD COLUMN study_year SMALLINT;
UPDATE demographics SET study_year=2018;


\copy demographics(study_id, site_id, age_days, sex, ethnicity, race, zip_marker, numerator) FROM 'pilot/input/nm/VaultDB_DEMOGRAPHIC_BASELINE_2019.csv' CSV HEADER
UPDATE demographics SET study_year=2019 WHERE study_year IS NULL;

\copy demographics(study_id, site_id, age_days, sex, ethnicity, race, zip_marker, numerator) FROM 'pilot/input/nm/VaultDB_DEMOGRAPHIC_BASELINE_2020.csv' CSV HEADER
UPDATE demographics SET study_year=2020 WHERE study_year IS NULL;

-- map pid to int by removing vaultdb prefix
UPDATE demographics SET study_id=REPLACE(study_id, 'VAULTDB','');
ALTER TABLE demographics ADD COLUMN pat_id INT;
UPDATE demographics SET pat_id=study_id::INT;
ALTER TABLE demographics DROP COLUMN study_id;

-- all zips are empty in this data
ALTER TABLE demographics DROP COLUMN zip_marker;
 
-- ethnicity:
-- Yes/No/R (refused)/NI (no info)/UN (unknown)/OT (other)
-- some have conflicting info on this (R --> N) (NI --> N), OT --> other
-- replace refused and NI values with defined ones where possible
UPDATE demographics d1 SET ethnicity=d2.ethnicity FROM demographics d2  WHERE d1.pat_id = d2.pat_id 
       		       	   			       		    	AND (d1.ethnicity = 'R' OR d1.ethnicity = 'NI') -- existing ethnicity value is undetermined
       		       	   			       		    	AND (d2.ethnicity = 'Y' OR d2.ethnicity = 'N' OR d2.ethnicity = 'OT'); -- matching one is Y/N

-- align it with the others
UPDATE demographics SET ethnicity = 'U' WHERE ethnicity <> 'Y' AND ethnicity <> 'N';

-- race: (from PCORI CDM)
-- 01=American Indian or Alaska Native
-- 02=Asian
-- 03=Black or African American
-- 04=Native Hawaiian or Other Pacific Islander
-- 05=White
-- 06=Multiple race
-- 07=Refuse to answer (or unknown, added by JMR)
-- NI=No information
-- UN=Unknown
-- OT=Other

-- similar issue with inconsistencies in race.
-- want to replace 07, NI, UN with well-defined vals where possible
UPDATE demographics d1 SET race=d2.race FROM demographics d2  WHERE d1.pat_id = d2.pat_id  AND d1.site_id = d2.site_id
       		       	   			       		    	AND (d1.race IN ('07', 'NI', 'UN')) -- existing race value is undetermined
       		       	   			       		    	AND (d2.race IN ('01', '02', '03', '04', '05', '06', 'OT')); -- matching one is defined above

-- if we have both a NI and a "refuse" (07)  for a patient, pick refuse so we don't double count them.
UPDATE demographics d1 SET race=d2.race FROM demographics d2  WHERE d1.pat_id = d2.pat_id  AND d1.site_id = d2.site_id
       		       	   		     		            AND d1.race='NI' AND d2.race='07';



-- putting together unknown/no info/other/refused
UPDATE demographics SET race='07' WHERE race NOT IN ('01', '02', '03', '04', '05', '06', '07');

-- 07 = refuse to answer, but we will extend it to unknown since these entries have more than one value for race
WITH conflicting_race AS (
     SELECT DISTINCT d1.pat_id
     FROM demographics d1 JOIN demographics d2 ON d1.pat_id = d2.pat_id AND d1.race <> d2.race)
UPDATE demographics SET race='07'
  WHERE pat_id IN (SELECT * FROM conflicting_race);

ALTER TABLE demographics ALTER COLUMN race TYPE char(1) USING substring(race, 2, 1);
ALTER TABLE demographics ADD COLUMN age_strata char(1);

UPDATE demographics SET age_strata = CASE WHEN age_days <= 28*365 THEN '1'
                WHEN age_days > 28*365 AND age_days <= 39*365 THEN '2'
              WHEN age_days > 39*365  AND age_days <= 50*365 THEN '3'
              WHEN age_days > 50*365 AND age_days <= 61*365 THEN '4'
              WHEN age_days > 61*365 AND age_days <= 72*365 THEN '5'
                WHEN age_days > 72*365  AND age_days <= 83*365 THEN '6'
                ELSE '7' END;

ALTER TABLE demographics DROP COLUMN age_days;


UPDATE demographics SET sex='U' WHERE sex='UN';
ALTER TABLE demographics ALTER COLUMN sex TYPE char(1);






-- expected # of rows: 
-- From: SELECT COUNT(*) FROM (SELECT DISTINCT pat_id, study_year, site_id FROM demographics) t;
-- observed in this tmp table:
-- SELECT COUNT(*) FROM tmp;

DROP TABLE IF EXISTS tmp;
SELECT DISTINCT d1.pat_id, d1.study_year, d1.site_id, d1.age_strata, d1.sex, d1.ethnicity, d1.race,  COALESCE(d1.numerator, d2.numerator) numerator
  INTO tmp
FROM demographics d1 LEFT JOIN demographics d2 ON d1.pat_id = d2.pat_id  AND d1.study_year = d2.study_year
                                                      AND d1.site_id = d2.site_id
                                                      AND d1.age_strata = d2.age_strata
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
-- ********************
DROP TABLE IF EXISTS demographics;
ALTER TABLE tmp RENAME TO demographics;

-- ********************
UPDATE demographics SET numerator=false where numerator IS NULL;
ALTER TABLE demographics ALTER COLUMN  ethnicity TYPE char(1);

-- DISTINCT pat_id, age_strata, sex, race, ethnicity, numerator, study_year from demo
-- 746949
-- distinct pat_id, study_year:
-- 746941
-- 8 different

-- population labels: distinct pat_id, study_year
-- 769106 -- have a few spares?
-- have about 22k denom_excls to share
-- may need further multi-site joins for those - TODO later
-- some of these are likely being held in for denom exclusion only.
-- need a left join to keep them w/o demo data.
-- 3 cases:
-- 1. denom_excl - no demo data, excluded from study - retain in patient table for denom_excl - free to make up dummy data for rest
-- 2. appears in both - no dupes
-- 3. appears in both with dupe - data ambiguous, keep in both numerator and denominator


