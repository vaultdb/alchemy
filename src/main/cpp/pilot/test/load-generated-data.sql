-- stitch it all together into a single DB for validation
\c enrich_htn_unioned
DROP TABLE IF EXISTS patient;
CREATE TABLE patient (
    patid int,
    study_year int,
    zip_marker varchar(3),
    age_days integer,
    sex varchar(2),
    ethnicity varchar(2), -- 0 || 1
    race varchar(2),
    numerator bool default null,
    denominator bool default null,
    denom_excl bool default null,
    site_id int);

\copy patient FROM 'alice-patient.csv' CSV
\copy patient FROM 'bob-patient.csv' CSV
\copy patient FROM 'chi-patient.csv' CSV


