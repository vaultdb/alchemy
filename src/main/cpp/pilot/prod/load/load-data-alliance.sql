\c enrich_htn_prod
\set ECHO all

-- run this after pilot/prod/load/alliance/clean.sh

\i 'pilot/prod/load/alliance/load-demographics.sql'
\i 'pilot/prod/load/alliance/load-population-labels.sql'
\i 'pilot/prod/load/alliance/assemble-patient-table.sql'

\copy (SELECT pat_id, age_strata, sex, ethnicity, race, numerator, denom_excl FROM patient WHERE study_year=2018 ORDER BY pat_id) TO 'pilot/input/2018/alliance.csv' CSV
\copy (SELECT  pat_id, age_strata, sex, ethnicity, race, numerator, denom_excl FROM patient WHERE study_year=2019 ORDER BY pat_id) TO 'pilot/input/2019/alliance.csv' CSV
\copy (SELECT  pat_id, age_strata, sex, ethnicity, race, numerator, denom_excl FROM patient WHERE study_year=2020 ORDER BY pat_id) TO 'pilot/input/2020/alliance.csv' CSV


\copy (SELECT DISTINCT  pat_id, min(age_strata) age_strata, sex, ethnicity, race, numerator, denom_excl FROM patient     GROUP BY pat_id, sex, ethnicity, race, numerator, denom_excl ORDER BY pat_id) TO 'pilot/input/all/alliance.csv' CSV
