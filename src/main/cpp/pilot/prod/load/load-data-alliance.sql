\c enrich_htn_prod
\set ECHO all

-- run this after pilot/prod/load/alliance/clean.sh

\i 'pilot/prod/load/alliance/load-demographics.sql'
\i 'pilot/prod/load/alliance/load-population-labels.sql'
\i 'pilot/prod/load/alliance/assemble-patient-table.sql'



\copy (SELECT pat_id, age_strata, sex, ethnicity, race, numerator, denom_excl FROM patient WHERE study_year=2018 ORDER BY pat_id) TO 'pilot/input/clean/2018.csv' CSV
\copy (SELECT  pat_id, age_strata, sex, ethnicity, race, numerator, denom_excl FROM patient WHERE study_year=2019 ORDER BY pat_id) TO 'pilot/input/clean/2019.csv' CSV
\copy (SELECT  pat_id, age_strata, sex, ethnicity, race, numerator, denom_excl FROM patient WHERE study_year=2020 ORDER BY pat_id) TO 'pilot/input/clean/2020.csv' CSV


\copy (SELECT DISTINCT  pat_id, min(age_strata) age_strata, sex, ethnicity, race, numerator, denom_excl FROM patient     GROUP BY pat_id, sex, ethnicity, race, numerator, denom_excl ORDER BY pat_id) TO 'pilot/input/clean/all.csv' CSV


\copy (SELECT DISTINCT pat_id, study_year, 'AC' site_id FROM patient ORDER BY pat_id, study_year) TO 'pilot/output/alliance_pat_ids.csv' CSV;

\set ECHO none
DROP TABLE IF EXISTS multisite;
CREATE TEMP TABLE multisite (pat_id INT, study_year int);
\copy multisite FROM 'pilot/input/alliance-multisite.csv' CSV HEADER
ALTER TABLE patient ADD COLUMN multisite bool DEFAULT false;

UPDATE patient p SET multisite=true
FROM multisite m
WHERE m.pat_id = p.pat_id AND m.study_year = p.study_year;

\i 'pilot/prod/load/verify-domain.sql'
\i 'pilot/prod/load/generate-demographics-domain.sql'
