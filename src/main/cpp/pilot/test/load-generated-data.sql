-- stitch it all together into a single DB for validation
\c enrich_htn_unioned
DROP TABLE IF EXISTS patient;
CREATE TABLE patient (
    patid int,
    zip_marker varchar(3),
    age_days integer,
    sex varchar(1),
    ethnicity bool, -- 0 || 1
    race int,
    numerator int default null,
    denom_excl int default null,
    site_id int);

\copy patient FROM 'alice-patient.csv' CSV
\copy patient FROM 'bob-patient.csv' CSV
\copy patient FROM 'chi-patient.csv' CSV


