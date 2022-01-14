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
-- \copy population_labels(measure_handle, study_year, study_id, denom, denom_excl, numerator) FROM 'pilot/input/alliance/population-labels-1.csv' CSV HEADER;

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

-- eliminate duplicate population labels
DROP TABLE IF EXISTS tmp;

SELECT DISTINCT pat_id, study_year, site_id, numerator, denom_excl
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
-- SELECT *
-- FROM population_labels
-- WHERE (pat_id, study_year) IN (SELECT * FROM duplicates)
-- ORDER BY pat_id, study_year;


-- count the conflicting sets
-- SELECT COUNT(*) FROM (
-- SELECT pat_id, study_year, COUNT(*)
-- FROM population_labels
-- GROUP BY pat_id, study_year
-- HAVING COUNT(*) > 1
-- ) t;

-- compare to whole size
-- SELECT COUNT(*)
-- FROM population_labels;

-- 71,490 population labels after loading just first measures file
-- have about 324 rows with conflicting info (162 pairs)

-- if we just load the second file, no conflicts
-- 69,497 records

-- if we load both, we get 16,428 pairs of conflicts and 87760 measurements
-- just go with the second one for now
