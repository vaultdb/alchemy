-- stitch it all together into a single DB for validation
-- start with pilot/prod/clean-nulls.sh first
\c enrich_htn_prod
\set ECHO all


\i 'pilot/prod/load/nm/load-demographics.sql'
\i 'pilot/prod/load/nm/load-population-labels.sql'
\i 'pilot/prod/load/nm/assemble-patient-table.sql'


\copy (SELECT DISTINCT pat_id, study_year, 'NM' site_id FROM patient ORDER BY pat_id, study_year) TO 'pilot/output/nm_pat_ids.csv' CSV;

DROP TABLE IF EXISTS multisite;
CREATE TEMP TABLE multisite (pat_id INT, study_year int);
\copy multisite FROM 'pilot/input/nm-multisite.csv' CSV HEADER
ALTER TABLE patient ADD COLUMN multisite bool DEFAULT false;

UPDATE patient p SET multisite=true
FROM multisite m
WHERE m.pat_id = p.pat_id AND m.study_year = p.study_year;



\copy (SELECT pat_id, age_strata, sex, ethnicity, race, numerator, denom_excl FROM patient WHERE study_year=2018 ORDER BY pat_id) TO 'pilot/input/clean/2018.csv' CSV
\copy (SELECT  pat_id, age_strata, sex, ethnicity, race, numerator, denom_excl FROM patient WHERE study_year=2019 ORDER BY pat_id) TO 'pilot/input/clean/2019.csv' CSV
\copy (SELECT  pat_id, age_strata, sex, ethnicity, race, numerator, denom_excl FROM patient WHERE study_year=2020 ORDER BY pat_id) TO 'pilot/input/clean/2020.csv' CSV
\copy (SELECT  pat_id, age_strata, sex, ethnicity, race, numerator, denom_excl FROM patient  ORDER BY pat_id, study_year) TO 'pilot/input/clean/all.csv' CSV


\copy (SELECT pat_id, age_strata, sex, ethnicity, race, numerator, denom_excl FROM patient WHERE study_year=2018 AND multisite ORDER BY pat_id) TO 'pilot/input/clean/2018-multisite.csv' CSV
\copy (SELECT  pat_id, age_strata, sex, ethnicity, race, numerator, denom_excl FROM patient WHERE study_year=2019 AND multisite ORDER BY pat_id) TO 'pilot/input/clean/2019-multisite.csv' CSV
\copy (SELECT  pat_id, age_strata, sex, ethnicity, race, numerator, denom_excl FROM patient WHERE study_year=2020 AND multisite ORDER BY pat_id) TO 'pilot/input/clean/2020-multisite.csv' CSV
\copy (SELECT  pat_id, age_strata, sex, ethnicity, race, numerator, denom_excl FROM patient WHERE multisite ORDER BY pat_id, study_year) TO 'pilot/input/clean/all-multisite.csv' CSV



\set ECHO none

\i 'pilot/prod/load/generate-demographics-domain.sql'
\i 'pilot/prod/load/verify-domain.sql'
