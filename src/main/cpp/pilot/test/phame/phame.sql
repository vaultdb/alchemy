-- JMR: these are notes to describe the layout of the PHAME study and how we are adapting it to Catalyst
-- these aren't meant to be runnable queries.
-- save to: `phame_cohort_counts.csv`
-- Table A1
-- phame_cohort_counts(desc:varchar(42), count:int64, site_id:int32)
SELECT *, 'my_site' as SITE_ID
INTO phame_cohort_counts
FROM (            ------------<------------      Specify Site Name
    SELECT 'Total Unique Patients at Site' as Attribute, COUNT(DISTINCT PATID) n FROM demographic
    UNION ALL
    SELECT 'Total Unique Patients before Randomization' as Attribute, COUNT(DISTINCT CAP_ID) n FROM study_population
    UNION ALL
    SELECT 'Total Unique Patients after Randomization' as Attribute, COUNT(DISTINCT CAP_ID) n FROM randomly_stratified
    UNION ALL
    SELECT 'Female - Age betwn 18-30' as Attribute, COUNT(DISTINCT CAP_ID) n FROM randomly_stratified WHERE SEX = 'F' AND AGE_CAT = '01'
    UNION ALL
    SELECT 'Male - Age betwn 18-30' as Attribute, COUNT(DISTINCT CAP_ID) n FROM randomly_stratified WHERE SEX = 'M' AND AGE_CAT = '01'
    UNION ALL
    SELECT 'Female - Age betwn 31-40' as Attribute, COUNT(DISTINCT CAP_ID) n  FROM randomly_stratified WHERE SEX = 'F' AND AGE_CAT = '02'
    UNION ALL
    SELECT 'Male - Age betwn 31-40' as Attribute, COUNT(DISTINCT CAP_ID) n  FROM randomly_stratified WHERE SEX = 'M' AND AGE_CAT = '02'
    UNION ALL
    SELECT 'Female - Age betwn 41-50' as Attribute, COUNT(DISTINCT CAP_ID) n  FROM randomly_stratified WHERE SEX = 'F' AND AGE_CAT = '03'
    UNION ALL
    SELECT 'Male - Age betwn 41-50' as Attribute, COUNT(DISTINCT CAP_ID) n  FROM randomly_stratified WHERE SEX = 'M' AND AGE_CAT = '03'
    UNION ALL
    SELECT 'Female - Age betwn 51-60' as Attribute, COUNT(DISTINCT CAP_ID) n  FROM randomly_stratified WHERE SEX = 'F' AND AGE_CAT = '04'
    UNION ALL
    SELECT 'Male - Age betwn 51-60' as Attribute, COUNT(DISTINCT CAP_ID) n FROM randomly_stratified WHERE SEX = 'M' AND AGE_CAT = '04'
    UNION ALL
    SELECT 'Female - Age betwn 61-70' as Attribute, COUNT(DISTINCT CAP_ID) n  FROM randomly_stratified WHERE SEX = 'F' AND AGE_CAT = '05'
    UNION ALL
    SELECT 'Male - Age betwn 61-70' as Attribute, COUNT(DISTINCT CAP_ID) n  FROM randomly_stratified WHERE SEX = 'M' AND AGE_CAT = '05'
    UNION ALL
    SELECT 'Female - Age betwn >70' as Attribute, COUNT(DISTINCT CAP_ID) n  FROM randomly_stratified WHERE SEX = 'F' AND AGE_CAT = '06'
    UNION ALL
    SELECT 'Male - Age betwn >70' as Attribute, COUNT(DISTINCT CAP_ID) n  FROM randomly_stratified WHERE SEX = 'M' AND AGE_CAT = '06'
    ) a;  -- save output as PHAME_COHORT_COUNTS.csv


-- source schema:
-- Table B4
-- PHAME_DEMO: patid, age, gender, race, hispanic, zip, payer_primary, payer_secondary
-- ***Save to phame_denominator.csv
-- schema:
-- phame_denominator(zip:char(5), total:int64, black:int64, white:int64, hispanic:int64, asian:int64, blacke_male:int64, white_male:int64, hispanic_male:int64, asian_male:int64, black_female:int64, white_female:int64, hispanic_female:int64, asian_female:int64)
SELECT ZIP,
       COUNT(DISTINCT PATID) AS TOTAL_N_POP,
       SUM(CASE WHEN RACE='Black  or  African  American' THEN 1 ELSE 0 END) AS N_BLACK_POP,
       SUM(CASE WHEN RACE='White' THEN 1 ELSE 0 END) AS N_WHITE_POP,
       SUM(CASE WHEN HISPANIC='Hispanic or Latino' THEN 1 ELSE 0 END) AS N_HISPANIC_POP,
       SUM(CASE WHEN RACE='Asian' THEN 1 ELSE 0 END) AS N_ASIAN_POP,
       SUM(CASE WHEN RACE='Black  or  African  American' AND GENDER='Male' THEN 1 ELSE 0 END) AS N_BLACK_MALE_POP,
       SUM(CASE WHEN RACE='White' AND GENDER='Male' THEN 1 ELSE 0 END) AS N_WHITE_MALE_POP,
       SUM(CASE WHEN HISPANIC='Hispanic or Latino' AND GENDER='Male' THEN 1 ELSE 0 END) AS N_HISPANIC_MALE_POP,
       SUM(CASE WHEN RACE='Asian' AND GENDER='Male' THEN 1 ELSE 0 END) AS N_ASIAN_MALE_POP,
       SUM(CASE WHEN RACE='Black  or  African  American' AND GENDER='Female' THEN 1 ELSE 0 END) AS N_BLACK_FEMALE_POP,
       SUM(CASE WHEN RACE='White' AND GENDER='Female' THEN 1 ELSE 0 END) AS N_WHITE_FEMALE_POP,
       SUM(CASE WHEN HISPANIC='Hispanic or Latino' AND GENDER='Female' THEN 1 ELSE 0 END) AS N_HISPANIC_FEMALE_POP,
       SUM(CASE WHEN RACE='Asian' AND GENDER='Female' THEN 1 ELSE 0 END) AS N_ASIAN_FEMALE_POP
INTO phame_denominator_catalyst
FROM phame_demo
GROUP BY ZIP
ORDER BY TOTAL_N_POP DESC;

--  copy of phame_demo
SELECT *  INTO phame_demo_catalyst FROM phame_demo;
-- add the age category (+ other cols for later coding)
ALTER TABLE #phame_demo_catalyst ADD age_cat CHAR(1), SEX CHAR(1),  race_coded CHAR(1), ethnicity CHAR(1), zip_code CHAR(5), --primary_payer_coded CHAR(1), secondary_payer_coded CHAR(1);
    payer_primary_coded CHAR(1), payer_secondary_coded CHAR(1);
UPDATE #phame_demo_catalyst SET age_cat=CASE
    WHEN AGE BETWEEN 18 AND 30 THEN '1'
    WHEN AGE BETWEEN 31 AND 40 THEN '2'
    WHEN AGE BETWEEN 41 AND 50 THEN '3'
    WHEN AGE BETWEEN 51 AND 60 THEN '4'
    WHEN AGE BETWEEN 61 AND 70 THEN '5'
    WHEN AGE IS NULL THEN '0'
    WHEN AGE > 70 THEN '6' END;

ALTER TABLE #phame_demo_catalyst DROP COLUMN age;

-- revert sex to M/F/N
-- N is for non-binary
UPDATE #phame_demo_catalyst  SET SEX=
    CASE WHEN GENDER='Male' THEN 'M'
    WHEN GENDER='Female' THEN 'F'
    WHEN GENDER IS NULL THEN '0'
    ELSE 'N' END;

ALTER TABLE #phame_demo_catalyst DROP COLUMN GENDER;
ALTER TABLE #phame_demo_catalyst RENAME COLUMN SEX TO GENDER;


UPDATE phame_demo_catalyst SET race_coded =
    CASE WHEN race='American  Indian  or  Alaska  Native' THEN '1'
    WHEN race='Asian' THEN '2'
    WHEN race='Black  or  African  American' THEN '3'
    WHEN  race='Native  Hawaiian  or  Other  Pacific  Islander' THEN '4'
    WHEN race='White' THEN '5'
    WHEN race='Multiple  race' THEN '6'
    WHEN race='Refuse  to  answer' THEN '7'
    WHEN race='No  information' THEN '8'
    WHEN race IS NULL THEN '8' -- literally no information
    WHEN race='Unknown' THEN '9'
    ELSE '0' END; -- for 'Other'

ALTER TABLE #phame_demo_catalyst DROP COLUMN race;
--ALTER TABLE #phame_demo_catalyst RENAME race_coded TO race;

UPDATE #phame_demo_catalyst SET ethnicity=
    CASE WHEN hispanic='Hispanic or Latino' THEN '1'
    WHEN hispanic='Not Hispanic or Latino' THEN '2'
    WHEN hispanic='Refuse  to  answer' THEN '3'
    WHEN hispanic='No  information' THEN '4'
    WHEN hispanic='Unknown' THEN '5'
    WHEN hispanic IS NULL THEN 0
    ELSE '6' END; -- 5 --> 'Other'

ALTER TABLE #phame_demo_catalyst DROP COLUMN hispanic;

-- all zips are in the domain of starting with '6' and having 5 characters
-- but zip code in original schema is a lot longer - truncate it by taking the first 5 characters
-- can put this in an int32, but keeping it simple for now!
UPDATE #phame_demo_catalyst SET zip_code = LEFT(zip, 5);
UPDATE #phame_demo_catalyst SET zip_code='00000' WHERE zip IS NULL;

ALTER TABLE #phame_demo_catalyst DROP COLUMN zip;


UPDATE #phame_demo_catalyst SET payer_primary_coded =
    CASE WHEN payer_primary='Medicare' THEN '1'
    WHEN payer_primary='Private/Commercial' THEN '2'
    WHEN payer_primary='No  information' THEN '3'
    WHEN payer_primary='Other' THEN '4'
    WHEN payer_primary='Unknown' THEN '5'
    WHEN payer_primary='Other' THEN '6'
    WHEN payer_primary IS NULL THEN '0' END;


UPDATE #phame_demo_catalyst SET payer_secondary_coded =
    CASE WHEN payer_secondary='Medicare' THEN '1'
    WHEN payer_secondary='Private/Commercial' THEN '2'
    WHEN payer_secondary='No  information' THEN '3'
    WHEN payer_secondary='Other' THEN '4'
    WHEN payer_secondary='Unknown' THEN '5'
    WHEN payer_secondary='Other' THEN '6'
    WHEN payer_secondary IS NULL THEN '0' END;

ALTER TABLE #phame_demo_catalyst DROP COLUMN payer_primary;
ALTER TABLE #phame_demo_catalyst DROP COLUMN payer_secondary;

-- source schema:
-- PHAME_DIAGNOSIS: patid, zip, dx_diabetes, dx_diabetes_dt, dx_hypertension, dx_hypertension_dt, dx_cervical_cancer, dx_cervical_cancer_dt, dx_breast_cancer, dx_breast_cancer_dt, dx_lung_cancer, dx_lung_cancer_dt, dx_colorectal_cancer, dx_colorectal_cancer_dt
-- phame_diagnosis already deduped earlier
-- this is just simplifying our setup
SELECT patid, dx_diabetes, dx_hypertension, dx_cervical_cancer, dx_breast_cancer, dx_lung_cancer, dx_colorectal_cancer
INTO phame_diagnosis_catalyst
FROM phame_diagnosis;



-- roll up cube with diagnoses
-- everyone to generate this.
-- if data partner is contributing patient-level data, they will send this cube to the investigator to validate our results
-- if a site is contributing aggregate-level only, send this to MPC

-- Site descriptions
-- detailed site:  contributing patient level data
-- aggregate site: ones contributing rollups alone

SELECT age_cat, sex, race_coded, ethnicity, demo.zip_code, payer_primary_coded, payer_secondary_coded, COUNT(demo.patid) patient_cnt,
       SUM(dx_diabetes) diabetes_cnt, SUM(dx_hypertension) hypertension_cnt, SUM(dx_cervical_cancer) cervical_cancer_cnt,
       SUM(dx_breast_cancer) breast_cancer_cnt, SUM(dx_lung_cancer) lung_cancer_cnt, SUM(dx_colorectal_cancer) colorectal_cancer_cnt
INTO #phame_rollup_catalyst
FROM #phame_demo_catalyst demo  LEFT JOIN #phame_diagnosis dx ON  demo.patid = dx.patid
GROUP BY age_cat, sex, race_coded, ethnicity, demo.zip_code, payer_primary_coded, payer_secondary_coded
ORDER BY age_cat, sex, race_coded, ethnicity, demo.zip_code, payer_primary_coded, payer_secondary_coded;


-- DETAILED ONLY
-- schema:
--     age_cat: \in [0, 6]
--     gender: ('M', 'F', '0', 'N')
--     race:
-- (patid:int32, age_cat:char(1), gender:char(1), race:char(1), ethnicity:char(1), zip:char(5), payer_primary:char(1), payer_secondary:char(1))
select * from phame_demo_catalyst;

-- DETAILED ONLY
-- save phame_diagnosis_catalyst as phame_diagnosis.csv
-- schema: (implicitly casting study vars to bools, use count in MPC to add them up)
-- patid:int32, dx_diabetes:bool, dx_hypertension:bool, dx_cervical_cancer:bool, dx_breast_cancer:bool, dx_lung_cancer:bool, dx_colorectal_cancer:bool

select * from phame_diagnosis_catalyst;

-- ALL SITES SEND THIS
-- save output as PHAME_COHORT_COUNTS.csv
-- schema:
-- (attribute_desc:varchar(42), n:int32)
-- Table A1
SELECT * FROM #phame_cohort_counts;

-- ALL SITES SEND THIS
-- *** save phame_rollup_catalyst to phame_rollup.csv
-- schema:
--age_cat:char(1), gender:char(1), race:char(1), ethnicity:char(1), zip:char(5), payer_primary:char(1), payer_secondary:char(1), patient_cnt:int64, diabetes_cnt:int64, hypertension_cnt:int64, cervical_cancer_cnt:int64, breast_cancer_cnt:int64, lung_cancer_cnt:int64, colorectal_cancer_cnt:int64
select * from #phame_rollup_catalyst;



