-- stitch it all together into a single DB for validation
-- start with pilot/prod/clean-nulls.sh first
\c enrich_htn_prod
\set ECHO all


\i 'pilot/prod/load/nm/load-demographics.sql'
\i 'pilot/prod/load/nm/load-population-labels.sql'
\i 'pilot/prod/load/nm/assemble-patient-table.sql'



\copy (SELECT pat_id, age_strata, sex,  ethnicity, race, numerator, denom_excl FROM patient WHERE study_year=2018 ORDER BY pat_id, site_id) TO 'pilot/input/alice-prod-patient.csv' CSV
\copy (SELECT  pat_id, age_strata, sex,  ethnicity, race, numerator, denom_excl FROM patient WHERE study_year=2019 ORDER BY pat_id, site_id) TO 'pilot/input/bob-prod-patient.csv' CSV
\copy (SELECT  pat_id, age_strata, sex,  ethnicity, race, numerator, denom_excl FROM patient WHERE study_year=2020 ORDER BY pat_id, site_id) TO 'pilot/input/chi-prod-patient.csv' CSV

\copy (SELECT  pat_id, age_strata, sex,  ethnicity, race, numerator, denom_excl FROM patient WHERE study_year=2018 ORDER BY pat_id, site_id) TO 'pilot/input/clean/2018.csv' CSV
\copy (SELECT  pat_id, age_strata, sex,  ethnicity, race, numerator, denom_excl FROM patient WHERE study_year=2019 ORDER BY pat_id, site_id) TO 'pilot/input/clean/2019.csv' CSV
\copy (SELECT  pat_id, age_strata, sex,  ethnicity, race, numerator, denom_excl  FROM patient WHERE study_year=2020 ORDER BY pat_id, site_id) TO 'pilot/input/clean/2020.csv' CSV

\copy (SELECT DISTINCT  pat_id, min(age_strata) age_strata, sex, ethnicity, race, numerator, denom_excl FROM patient     GROUP BY pat_id, sex, ethnicity, race, numerator, denom_excl ORDER BY pat_id) TO 'pilot/input/clean/all.csv' CSV



\i 'pilot/prod/load/verify-domain.sql'
