\echo 'Loading population labels...'

DROP TABLE IF EXISTS population_labels;

CREATE TABLE population_labels(
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
 \copy population_labels(measure_handle, study_year, study_id, denom, denom_excl, numerator) FROM 'pilot/input/alliance/population-labels-1.csv' CSV HEADER;

-- 110810 rows
\copy population_labels(measure_handle, study_year, study_id, denom, denom_excl, denom_except, numerator) FROM 'pilot/input/alliance/population-labels-2.csv' CSV HEADER;



\echo 'Cleaning...'

-- remove rows that are not from study years
DELETE FROM population_labels
WHERE study_year NOT IN (2018, 2019, 2020);



-- map pid to int by removing vaultdb prefix
UPDATE population_labels SET study_id=REPLACE(study_id, 'VAULTDB','');
UPDATE population_labels SET pat_id=study_id::INT;
-- drop old version of patient id
ALTER TABLE population_labels DROP COLUMN study_id;


-- this appears to be from 2 separate DBs.
-- if one has grounds for exclusion, we propogate that to the other

UPDATE population_labels p1
SET denom_excl=true
WHERE EXISTS (SELECT *
      FROM population_labels p2
      WHERE p1.pat_id = p2.pat_id AND p1.study_year = p2.study_year AND p2.denom_excl = true);
      
-- similarly if one has grounds for being in numerator (elevated BP), then propogate that to the other measurement source
UPDATE population_labels p1
SET numerator=true
WHERE EXISTS (SELECT *
      FROM population_labels p2
      WHERE p1.pat_id = p2.pat_id AND p1.study_year = p2.study_year AND p2.numerator = true);



-- eliminate duplicate population labels
DROP TABLE IF EXISTS tmp;
SELECT DISTINCT pat_id, study_year, site_id, numerator, denom, denom_excl
INTO tmp
FROM population_labels;


DROP TABLE IF EXISTS population_labels;
ALTER TABLE tmp RENAME TO population_labels;


-- show the conflicts
-- WITH duplicates AS (
--      SELECT pat_id, study_year
--      FROM population_labels
--      GROUP BY pat_id, study_year
--      HAVING COUNT(*) > 1)
-- SELECT measure_handle, pat_id, study_year, numerator, denom_excl
-- FROM population_labels_orig
-- WHERE (pat_id, study_year) IN (SELECT * FROM duplicates)
-- ORDER BY pat_id, study_year
-- LIMIT 20;



-- count the conflicting sets
-- \echo 'Ones with conflicting info'
-- SELECT COUNT(*) FROM (
-- SELECT pat_id, study_year, COUNT(*)
-- FROM population_labels
-- GROUP BY pat_id, study_year
-- HAVING COUNT(*) > 1
-- ) t;

-- \echo 'ones without conflicting info'
-- SELECT COUNT(*) FROM (
-- SELECT pat_id, study_year, COUNT(*)
-- FROM population_labels
-- GROUP BY pat_id, study_year
-- HAVING COUNT(*) = 1
-- ) t;

-- compare to whole size
-- \echo 'all pop labels'
-- SELECT COUNT(*)
-- FROM population_labels;

-- \echo 'Count with numerator conflict'
-- -- 30k for numerator conflict
-- WITH conflicting_numerator AS (
--      SELECT *
--      FROM  population_labels p1 JOIN population_labels p2 ON p1.pat_id = p2.pat_id
--      	   AND p1.study_year = p2.study_year
-- 	   WHERE p1.numerator <> p2.numerator)
-- SELECT COUNT(*)
-- FROM conflicting_numerator;


-- \echo 'Count with denom_excl conflict'
-- -- 2600 with denom excl conflict
-- WITH conflicting_denom_excl AS (
--      SELECT *
--      FROM  population_labels p1 JOIN population_labels p2 ON p1.pat_id = p2.pat_id
--      	   AND p1.study_year = p2.study_year
-- 	   WHERE p1.denom_excl <> p2.denom_excl)
-- SELECT COUNT(*)
-- FROM conflicting_denom_excl;


   




-- 71,490 population labels after loading just first measures file
-- have about 324 rows with conflicting info (162 pairs)

-- if we just load the second file, no conflicts
-- 69,497 records

-- if we load both, we get 16,428 pairs of conflicts and 87760 measurements
-- just go with the second one for now
