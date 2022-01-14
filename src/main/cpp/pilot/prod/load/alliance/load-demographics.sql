\echo 'Loading demographics'
\set echo all

-- STUDY_ID,AGE_2018,SEX,ETHINICITY,RACE,ZIPPREFIX,NUMERATOR
-- numerator missing in alliance demo
DROP TABLE IF EXISTS demographics;
CREATE TABLE demographics (
   pat_id int,
   study_id varchar,
   site_id varchar(2) DEFAULT 'AC',
   age_years_2015 integer, -- convert to age_days when we join with measurements
   sex varchar, 
   ethnicity varchar, 
   race varchar, -- can cast if we make all non-int cases '00'
   zipcode  varchar); 

   
\copy demographics(study_id, zipcode, age_years_2015, sex, ethnicity, race) FROM   'pilot/input/alliance/demographics.csv'  CSV HEADER


-- map pid to int by removing vaultdb prefix
UPDATE demographics SET study_id=REPLACE(study_id, 'VAULTDB','');
UPDATE demographics SET pat_id=study_id::INT;
ALTER TABLE demographics DROP COLUMN study_id;

-- get rid of NULLs since we haven't implemented them yet
-- ALTER TABLE demographics ADD COLUMN zip_marker varchar(3);
-- UPDATE demographics SET zip_marker=SUBSTRING(zipcode, 1, 3);
ALTER TABLE demographics DROP COLUMN zipcode;

-- ethnicity:
-- Yes/No/R (refused)/NI (no info)/UN (unknown)/OT (other)
-- some have conflicting info on this (R --> N) (NI --> N), OT --> other
-- replace refused and NI values with defined ones where possible

-- Alliance has domain: Hispanic, Non-Hispanic, NA
-- to match RU and NM convert to Y (Hispanic), N (Non-Hispanic), NI (all others)

UPDATE demographics SET ethnicity = 'Y' WHERE ethnicity = 'Hispanic';
UPDATE demographics SET ethnicity = 'N' WHERE ethnicity = 'Non-Hispanic';
UPDATE demographics SET ethnicity = 'U' WHERE ethnicity <> 'Y' AND ethnicity <> 'N';
ALTER TABLE demographics ALTER COLUMN ethnicity TYPE char(1);

-- join in ethnicity if we have it from another year
UPDATE demographics d1 SET ethnicity = d2.ethnicity
   FROM demographics d2
   WHERE d1.pat_id = d2.pat_id AND d1.site_id = d2.site_id
   	 AND d1.ethnicity = 'U' AND d2.ethnicity <> 'U';



-- sex
UPDATE demographics SET sex='F' WHERE sex = 'Female';
UPDATE demographics SET sex='M' WHERE sex = 'Male';
UPDATE demographics SET sex='U' WHERE sex <> 'M' AND sex <> 'F';
ALTER TABLE demographics ALTER COLUMN sex TYPE char(1);

UPDATE demographics d1 SET sex = d2.sex
   FROM demographics d2
   WHERE d1.pat_id = d2.pat_id AND d1.site_id = d2.site_id
   	 AND d1.sex = 'U' AND d2.sex <> 'U';



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
-- Alliance domain alignment
UPDATE demographics SET race='01' WHERE race='American Indian or Alaska Native';
UPDATE demographics SET race='02' WHERE race='Asian';
UPDATE demographics SET race='05' WHERE race='White';
UPDATE demographics SET race='03' WHERE race='Black or African American';
UPDATE demographics SET race='UN' WHERE race='NA';

-- similar issue with inconsistencies in race.
-- want to replace 07, NI, UN with well-defined vals where possible
UPDATE demographics d1 SET race=d2.race FROM demographics d2  WHERE d1.pat_id = d2.pat_id  AND d1.site_id = d2.site_id

       AND (d1.race = 'UN' OR d1.race = '07') -- existing race value is undetermined
        AND (d2.race IN ('01', '02', '03', '04', '05', '06')); -- matching one is defined above





-- expected # of rows: 
-- From: SELECT COUNT(*) FROM (SELECT DISTINCT pat_id,  site_id FROM demographics) t;
-- observed in this tmp table:
-- SELECT COUNT(*) FROM tmp;

DROP TABLE IF EXISTS tmp;
SELECT DISTINCT pat_id, site_id,  age_years_2015, sex, ethnicity, race
INTO tmp
FROM demographics;



-- mostly complexity with the race or ethnicity field.  Thee are sometimes direct contradictions like a patient being listed as black or white - perhaps their entry should read 06 ( multiple race)?  Don't have enough info for these, leave them alone.
--
DROP TABLE IF EXISTS demographics;
ALTER TABLE tmp RENAME TO demographics;

-- ID conflicts and reassign to unknown
WITH conflicting_ethnicity AS (
     SELECT DISTINCT pat_id
     FROM demographics d1
     WHERE ethnicity = 'Y' AND EXISTS (
     	   SELECT *
     	   FROM demographics d2
	   WHERE d1.pat_id = d2.pat_id AND d2.ethnicity = 'N'))
UPDATE demographics d SET ethnicity='U'
  FROM conflicting_ethnicity c
  WHERE d.pat_id=c.pat_id;


-- do the same thing with race
-- 07 = refuse to answer, but we will extend it to unknown
WITH conflicting_race AS (
     SELECT DISTINCT d1.pat_id
     FROM demographics d1 JOIN demographics d2 ON d1.pat_id = d2.pat_id AND d1.race <> d2.race)
UPDATE demographics SET race='07'
  WHERE pat_id IN (SELECT * FROM conflicting_race);

UPDATE demographics SET race='07' WHERE race = 'UN';
-- has two readings off by 1 year for this one
UPDATE demographics SET age_years_2015=52 WHERE pat_id=195825;

-- drop duplicate entries
DROP TABLE IF EXISTS tmp;
SELECT DISTINCT pat_id, site_id,  age_years_2015, sex, ethnicity, race
INTO tmp
FROM demographics;


DROP TABLE IF EXISTS demographics;
ALTER TABLE tmp RENAME TO demographics;


-- what are the remaining outliers?
-- SELECT d.*
-- FROM demographics d JOIN (SELECT pat_id FROM demographics GROUP BY pat_id HAVING COUNT(*) > 1) t
--     ON d.pat_id = t.pat_id
-- ORDER BY d.pat_id;


-- SELECT d1.pat_id, d1.age_years_2015, d2.age_years_2015, d1.race, d2.race, d1.sex, d2.sex, d1.ethnicity, d2.ethnicity
-- FROM demographics d1 JOIN demographics d2 ON d1.pat_id = d2.pat_id AND d1.ethnicity > d2.ethnicity
-- ORDER BY d1.pat_id;


