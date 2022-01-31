\i 'pilot/test/create-patient-table.sql'

-- stitch it all together into a single DB for validation
\copy patient(pat_id, study_year, age_strata, sex, ethnicity, race, numerator, denom_excl, site_id) FROM 'pilot/test/input/alice-patient.csv' CSV
\copy patient(pat_id, study_year, age_strata, sex, ethnicity, race, numerator, denom_excl, site_id) FROM 'pilot/test/input/bob-patient.csv' CSV
\copy patient(pat_id, study_year, age_strata, sex, ethnicity, race, numerator, denom_excl, site_id) FROM 'pilot/test/input/chi-patient.csv' CSV



UPDATE patient p1
SET multisite=true
WHERE EXISTS(
    SELECT *
    FROM patient p2
    WHERE p1.pat_id = p2.pat_id AND p1.study_year = p2.study_year AND p1.site_id <> p2.site_id);

\i  'pilot/prod/load/generate-demographics-domain.sql'
\i  'pilot/prod/load/add-hashes-to-patids.sql'

\copy (SELECT * FROM patient WHERE site_id = 1 ORDER BY pat_id, study_year) TO 'pilot/test/input/alice-db.csv' CSV
\copy (SELECT * FROM patient WHERE site_id = 2 ORDER BY pat_id, study_year) TO 'pilot/test/input/bob-db.csv' CSV
\copy (SELECT * FROM patient WHERE site_id = 3 ORDER BY pat_id, study_year) TO 'pilot/test/input/chi-db.csv' CSV

DROP DATABASE IF EXISTS enrich_htn_alice;
CREATE DATABASE enrich_htn_alice;
\c enrich_htn_alice
\i 'pilot/test/create-patient-table.sql'
\copy patient FROM 'pilot/test/input/alice-db.csv' CSV
\i  'pilot/prod/load/generate-demographics-domain.sql'


DROP DATABASE IF EXISTS enrich_htn_bob;
CREATE DATABASE enrich_htn_bob;
\c enrich_htn_bob
\i 'pilot/test/create-patient-table.sql'
\copy patient FROM 'pilot/test/input/bob-db.csv' CSV
\i  'pilot/prod/load/generate-demographics-domain.sql'



DROP DATABASE IF EXISTS enrich_htn_chi;
CREATE DATABASE enrich_htn_chi;
\c enrich_htn_chi
\i 'pilot/test/create-patient-table.sql'
\copy patient FROM 'pilot/test/input/chi-db.csv' CSV
\i  'pilot/prod/load/generate-demographics-domain.sql'

