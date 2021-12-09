\echo 'Loading population labels'
DROP TABLE IF EXISTS population_labels;

-- setup:
-- STUDY_ID,SITE_ID,NUMERATOR,DENOM_EXCL


-- before this, in the data directory (pilot/input/nm/ here) run:
--  for f in $(ls *.csv); do echo "processing $f" ; sed -i 's/,NULL/,/g' $f; done

-- For study, we basically have 3 groups:
-- numerator: study population (demographics.numerator = true)
-- denominator: baseline population - everyone not in denom_excl or where population_labels.numerator = false
-- denom_excl: folks who are excluded from the study entirely.
-- population_labels.numerator = true if patient in both numerator and denom groups

CREATE TABLE population_labels (
      study_id varchar,
      site_id varchar(2),
      numerator bool,
      denom_excl bool);

\copy population_labels FROM 'pilot/input/nm/VaultDB_POPULATION_LABELS_2018.csv' CSV HEADER
ALTER TABLE population_labels ADD COLUMN study_year INT;
UPDATE population_labels SET study_year=2018;

\copy population_labels(study_id, site_id, numerator, denom_excl) FROM 'pilot/input/nm/VaultDB_POPULATION_LABELS_2019.csv' CSV HEADER
UPDATE population_labels SET study_year=2019 WHERE study_year IS NULL;

\copy population_labels(study_id, site_id, numerator, denom_excl) FROM 'pilot/input/nm/VaultDB_POPULATION_LABELS_2020.csv' CSV HEADER
UPDATE population_labels SET study_year=2020 WHERE study_year IS NULL;

-- test for dupes with:
-- SELECT study_id, study_year, COUNT(*) FROM population_labels GROUP BY study_id, study_year HAVING COUNT(*) > 1;
-- delete the duplicates
-- DROP TABLE IF EXISTS population_labels_distinct;
-- SELECT DISTINCT study_id, site_id, study_year, numerator, denom_excl
-- INTO population_labels_distinct
-- FROM population_labels;

-- DROP TABLE population_labels;
-- ALTER TABLE population_labels_distinct RENAME TO population_labels;

-- map pid to int by removing vaultdb prefix
UPDATE population_labels SET study_id=REPLACE(study_id, 'VAULTDB','');
ALTER TABLE population_labels ADD COLUMN pat_id INT;
UPDATE population_labels SET pat_id=study_id::INT;
-- drop old version of patient id
ALTER TABLE population_labels DROP COLUMN study_id;

-- find duplicate ones:
-- SELECT *
-- FROM population_labels
-- WHERE (pat_id, study_year) IN (
--    SELECT pat_id, study_year
--        FROM population_labels
--	    GROUP BY pat_id, study_year
--	        HAVING COUNT(*) > 1)
--		ORDER BY pat_id;



DROP TABLE IF EXISTS tmp;

SELECT DISTINCT p1.pat_id, p1.study_year, p1.site_id, COALESCE(p1.numerator, p2.numerator) numerator, COALESCE(p1.denom_excl, p2.denom_excl) denom_excl
INTO tmp
FROM population_labels p1 LEFT JOIN population_labels p2 ON p1.pat_id = p2.pat_id AND p1.study_year = p2.study_year AND p1.site_id = p2.site_id
     AND  (p2.numerator IS NOT NULL OR p2.denom_excl IS NOT NULL);

DROP TABLE IF EXISTS population_labels;
ALTER TABLE tmp RENAME TO population_labels;


UPDATE population_labels SET numerator=false WHERE numerator IS NULL;
UPDATE population_labels SET denom_excl=false WHERE denom_excl IS NULL;
