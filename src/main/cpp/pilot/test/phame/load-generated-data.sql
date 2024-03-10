-- run after ./bin/generate_phame_data_n_parties
DROP TABLE IF EXISTS phame_demographic;
CREATE TABLE phame_demographic (patid int,
                         age_cat char(1),
                         gender char(1),
                         ethnicity char(1),
                         race char(1),
                            zip char(5),
                            payer_primary char(2),
                            payer_secondary char(2),
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

-- Next up:
-- configure sites 1 and 3 to be row-only
-- 2 and 4 are aggregate-only
