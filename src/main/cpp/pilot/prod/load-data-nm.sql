-- stitch it all together into a single DB for validation
-- start with pilot/prod/clean-nulls.sh first
\c enrich_htn_prod
\set ECHO all



\i 'pilot/prod/load-population-labels-nm.sql'
\i 'pilot/prod/load-demographics-nm.sql'
\i 'pilot/prod/assemble-patient-table-nm.sql'


\copy (SELECT * FROM patient WHERE study_year=2018 ORDER BY pat_id, site_id) TO 'pilot/input/alice-prod-patient.csv' CSV
\copy (SELECT * FROM patient WHERE study_year=2019 ORDER BY pat_id, site_id) TO 'pilot/input/bob-prod-patient.csv' CSV
\copy (SELECT * FROM patient WHERE study_year=2020 ORDER BY pat_id, site_id) TO 'pilot/input/chi-prod-patient.csv' CSV
