-- mock-up of the logic of HTN protocol
DROP TABLE IF EXISTS patient;
CREATE TABLE patient (
    patid int,
    zip_marker varchar(3),
    age_days integer,
    sex varchar(1),
    ethnicity int, -- 0 || 1
    race int,
    numerator int default null -- denotes null = false, 1 = true
);

DROP TABLE IF EXISTS  patient_exclusion;
CREATE TABLE patient_exclusion (
    patid int,
    numerator int default  null,
    denom_excl int default null
);

-- getting hashes out of band, patient_ids will be harmonized before we receive them

-- first harmonize the datasets to select patients that are not in denom_excl and that qualify for the numerator
-- unioned over the data of multiple sites
DROP TABLE IF EXISTS  unioned_patients;
 CREATE TABLE unioned_patients AS (
    WITH labeled as (
        SELECT patid, zip_marker, CASE WHEN age_days <= 28*365 THEN 0
                WHEN age_days > 28*365 AND age_days <= 39*365 THEN 1
              WHEN age_days > 39*365  AND age_days <= 50*365 THEN 2
              WHEN age_days > 50*365 AND age_days <= 61*365 THEN 3
              WHEN age_days > 61*365 AND age_days <= 72*365 THEN 4
                WHEN age_days > 72*365  AND age_days <= 83*365 THEN 5
                ELSE 6 END age_strata,
            sex, ethnicity, race, numerator
        FROM patient
        ORDER BY patid)
    SELECT p.patid, zip_marker, age_strata, sex, ethnicity, race, max(p.numerator) numerator, CASE WHEN count(*) > 1 THEN 1 else NULL END AS multisite
    FROM labeled p JOIN patient_exclusion pe ON p.patid = pe.patid
    GROUP BY p.patid, zip_marker, age_strata, sex, ethnicity, race
    HAVING max(denom_excl) IS NULL);

-- perform fine-grained aggregation
DROP TABLE IF EXISTS aggregated;
CREATE TEMP TABLE aggregated AS (
SELECT zip_marker, age_strata, sex, ethnicity, race, SUM(numerator), COUNT(*) denominator,
       SUM(CASE WHEN numerator IS NOT NULL THEN multisite ELSE NULL END)::float / SUM(numerator)::float proportion_numerator_multisite,
       SUM(multisite)::float / COUNT(*)::float proportion_denom_multisite
FROM unioned_patients
GROUP BY zip_marker, age_strata, sex, ethnicity, race);
