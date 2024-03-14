-- run after ./bin/generate_phame_data_n_parties
DROP TABLE IF EXISTS phame_demographic;
CREATE TABLE phame_demographic (patid int,
                         age_cat char(1),
                         gender char(1),
                         race char(1),
                        ethnicity char(1),
                        zip char(5),
                        payer_primary char(1),
                        payer_secondary char(1),
                        site_id int);


\copy phame_demographic FROM 'pilot/test/input/0/phame_demographic.csv' WITH DELIMITER ',';
\copy phame_demographic FROM 'pilot/test/input/1/phame_demographic.csv' WITH DELIMITER ',';
\copy phame_demographic FROM 'pilot/test/input/2/phame_demographic.csv' WITH DELIMITER ',';
\copy phame_demographic FROM 'pilot/test/input/3/phame_demographic.csv' WITH DELIMITER ',';


DROP TABLE IF EXISTS phame_diagnosis;
CREATE TABLE phame_diagnosis (
    patid int,
    dx_diabetes bool,
    dx_hypertension bool,
    dx_breast_cancer bool,
    dx_lung_cancer bool,
    dx_colorectal_cancer bool,
    dx_cervical_cancer bool,
    site_id int);

\copy phame_diagnosis FROM 'pilot/test/input/0/phame_diagnosis.csv' WITH DELIMITER ',';
\copy phame_diagnosis FROM 'pilot/test/input/1/phame_diagnosis.csv' WITH DELIMITER ',';
\copy phame_diagnosis FROM 'pilot/test/input/2/phame_diagnosis.csv' WITH DELIMITER ',';
\copy phame_diagnosis FROM 'pilot/test/input/3/phame_diagnosis.csv' WITH DELIMITER ',';


-- for phame_cohort_counts:
SELECT *
INTO phame_cohort_counts
FROM (            ------------<------------      Specify Site Name
    SELECT 'Total Unique Patients at Site' as Attribute, COUNT(DISTINCT PATID) n, site_id FROM phame_demographic GROUP BY site_id
    UNION ALL
    SELECT 'Total Unique Patients before Randomization' as Attribute, COUNT(DISTINCT patid) n, site_id FROM phame_demographic  GROUP BY site_id
    UNION ALL
    SELECT 'Total Unique Patients after Randomization' as Attribute, COUNT(DISTINCT patid) n, site_id FROM phame_demographic  GROUP BY site_id
    UNION ALL
    SELECT 'Female - Age betwn 18-30' as Attribute, COUNT(DISTINCT patid) n, site_id FROM phame_demographic WHERE gender = 'F' AND AGE_CAT = '1'  GROUP BY site_id
    UNION ALL
    SELECT 'Male - Age betwn 18-30' as Attribute, COUNT(DISTINCT patid) n, site_id FROM phame_demographic WHERE gender = 'M' AND AGE_CAT = '1'  GROUP BY site_id
    UNION ALL
    SELECT 'Female - Age betwn 31-40' as Attribute, COUNT(DISTINCT patid) n, site_id  FROM phame_demographic WHERE gender = 'F' AND AGE_CAT = '2'  GROUP BY site_id
    UNION ALL
    SELECT 'Male - Age betwn 31-40' as Attribute, COUNT(DISTINCT patid) n, site_id  FROM phame_demographic WHERE gender = 'M' AND AGE_CAT = '2'  GROUP BY site_id
    UNION ALL
    SELECT 'Female - Age betwn 41-50' as Attribute, COUNT(DISTINCT patid) n, site_id  FROM phame_demographic WHERE gender = 'F' AND AGE_CAT = '3'  GROUP BY site_id
    UNION ALL
    SELECT 'Male - Age betwn 41-50' as Attribute, COUNT(DISTINCT patid) n, site_id  FROM phame_demographic WHERE gender = 'M' AND AGE_CAT = '3'  GROUP BY site_id
    UNION ALL
    SELECT 'Female - Age betwn 51-60' as Attribute, COUNT(DISTINCT patid) n, site_id  FROM phame_demographic WHERE gender = 'F' AND AGE_CAT = '4'  GROUP BY site_id
    UNION ALL
    SELECT 'Male - Age betwn 51-60' as Attribute, COUNT(DISTINCT patid) n, site_id FROM phame_demographic WHERE gender = 'M' AND AGE_CAT = '4'  GROUP BY site_id
    UNION ALL
    SELECT 'Female - Age betwn 61-70' as Attribute, COUNT(DISTINCT patid) n, site_id  FROM phame_demographic WHERE gender = 'F' AND AGE_CAT = '5'  GROUP BY site_id
    UNION ALL
    SELECT 'Male - Age betwn 61-70' as Attribute, COUNT(DISTINCT patid) n, site_id  FROM phame_demographic WHERE gender = 'M' AND AGE_CAT = '5'  GROUP BY site_id
    UNION ALL
    SELECT 'Female - Age betwn >70' as Attribute, COUNT(DISTINCT patid) n, site_id  FROM phame_demographic WHERE gender = 'F' AND AGE_CAT = '6'  GROUP BY site_id
    UNION ALL
    SELECT 'Male - Age betwn >70' as Attribute, COUNT(DISTINCT patid) n, site_id  FROM phame_demographic WHERE gender = 'M' AND AGE_CAT = '6'  GROUP BY site_id
    ) a;

\copy (SELECT * FROM phame_cohort_counts WHERE site_id = 0) TO 'pilot/secret_shares/input/0/phame_cohort_counts.csv' WITH DELIMITER ',';
\copy (SELECT * FROM phame_cohort_counts WHERE site_id = 1) TO 'pilot/secret_shares/input/1/phame_cohort_counts.csv' WITH DELIMITER ',';
\copy (SELECT * FROM phame_cohort_counts WHERE site_id = 2) TO 'pilot/secret_shares/input/2/phame_cohort_counts.csv' WITH DELIMITER ',';
\copy (SELECT * FROM phame_cohort_counts WHERE site_id = 3) TO 'pilot/secret_shares/input/3/phame_cohort_counts.csv' WITH DELIMITER ',';

-- generate expected results
\copy (SELECT attribute, SUM(n) FROM phame_cohort_counts GROUP BY attribute ORDER BY attribute) TO 'pilot/study/phame/expected/phame_cohort_counts.csv' WITH DELIMITER ',';


-- site-at-a-time rollup
CREATE OR REPLACE FUNCTION rollup_query (sid INT)
    RETURNS TABLE (
        age_cat char(1),
        gender char(1),
        race char(1),
        ethnicity char(1),
        zip char(5),
        payer_primary char(1),
        payer_secondary char(1),
        patient_cnt bigint,
        diabetes_cnt bigint,
        hypertension_cnt bigint,
        cervical_cancer_cnt bigint,
        breast_cancer_cnt bigint,
        lung_cancer_cnt bigint,
        colorectal_cancer_cnt bigint)
AS $$
BEGIN
    RETURN QUERY
        SELECT demo.age_cat, demo.gender, demo.race, demo.ethnicity, demo.zip, demo.payer_primary, demo.payer_secondary, COUNT(demo.patid) patient_cnt, SUM(COALESCE(dx_diabetes, false)::INT) diabetes_cnt, SUM(COALESCE(dx_hypertension, false)::INT) hypertension_cnt, SUM(COALESCE(dx_cervical_cancer, false)::INT) cervical_cancer_cnt, SUM(COALESCE(dx_breast_cancer, false)::INT) breast_cancer_cnt, SUM(COALESCE(dx_lung_cancer, false)::INT) lung_cancer_cnt, SUM(COALESCE(dx_colorectal_cancer, false)::INT) colorectal_cancer_cnt

        FROM phame_demographic demo LEFT JOIN phame_diagnosis pd on demo.patid = pd.patid AND demo.site_id = pd.site_id
        WHERE demo.site_id = sid
        GROUP BY demo.age_cat, demo.gender, demo.race, demo.ethnicity, demo.zip, demo.payer_primary, demo.payer_secondary
        ORDER BY demo.age_cat, demo.gender, demo.race, demo.ethnicity, demo.zip, demo.payer_primary, demo.payer_secondary;
END; $$

    LANGUAGE 'plpgsql';

-- write each one out to a file
\copy (SELECT * FROM rollup_query(0)) TO 'pilot/secret_shares/input/0/phame_diagnosis_rollup.csv' WITH DELIMITER ',';
\copy (SELECT * FROM rollup_query(1)) TO 'pilot/secret_shares/input/1/phame_diagnosis_rollup.csv' WITH DELIMITER ',';
\copy (SELECT * FROM rollup_query(2)) TO 'pilot/secret_shares/input/2/phame_diagnosis_rollup.csv' WITH DELIMITER ',';
\copy (SELECT * FROM rollup_query(3)) TO 'pilot/secret_shares/input/3/phame_diagnosis_rollup.csv' WITH DELIMITER ',';



-- sites 1 and 3 are row-level contributors in our test
-- output their phame_demo and phame_diagnosis tables without site_id to reflect the setup in their SQL calls
\copy (SELECT patid, age_cat, gender, race, ethnicity, zip, payer_primary, payer_secondary FROM phame_demographic WHERE site_id = 1) TO 'pilot/secret_shares/input/1/phame_demographic.csv' WITH DELIMITER ',';
\copy (SELECT patid, dx_diabetes, dx_hypertension, dx_breast_cancer, dx_lung_cancer, dx_colorectal_cancer, dx_cervical_cancer FROM phame_diagnosis WHERE site_id = 1) TO 'pilot/secret_shares/input/1/phame_diagnosis.csv' WITH DELIMITER ',';
\copy (SELECT patid, age_cat, gender, race, ethnicity, zip, payer_primary, payer_secondary FROM phame_demographic WHERE site_id = 3) TO 'pilot/secret_shares/input/3/phame_demographic.csv' WITH DELIMITER ',';
\copy (SELECT patid, dx_diabetes, dx_hypertension, dx_breast_cancer, dx_lung_cancer, dx_colorectal_cancer, dx_cervical_cancer FROM phame_diagnosis WHERE site_id = 3) TO 'pilot/secret_shares/input/3/phame_diagnosis.csv' WITH DELIMITER ',';

-- generate expected results
\copy (SELECT demo.age_cat, demo.gender, demo.race, demo.ethnicity, demo.zip, demo.payer_primary, demo.payer_secondary,  COUNT(demo.patid) patient_cnt, SUM(COALESCE(dx_diabetes, false)::INT) diabetes_cnt, SUM(COALESCE(dx_hypertension, false)::INT) hypertension_cnt, SUM(COALESCE(dx_cervical_cancer, false)::INT) cervical_cancer_cnt, SUM(COALESCE(dx_breast_cancer, false)::INT) breast_cancer_cnt, SUM(COALESCE(dx_lung_cancer, false)::INT) lung_cancer_cnt, SUM(COALESCE(dx_colorectal_cancer, false)::INT) colorectal_cancer_cnt FROM phame_demographic demo LEFT JOIN phame_diagnosis pd on demo.patid = pd.patid AND demo.site_id = pd.site_id GROUP BY demo.age_cat, demo.gender, demo.race, demo.ethnicity, demo.zip, demo.payer_primary, demo.payer_secondary ORDER BY demo.age_cat, demo.gender, demo.race, demo.ethnicity, demo.zip, demo.payer_primary, demo.payer_secondary) TO 'pilot/study/phame/expected/phame_diagnosis_rollup.csv' WITH DELIMITER ','