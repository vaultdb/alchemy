-- phame_cohort_counts(desc:varchar(42), count:int64, site_id:int32)
SELECT *, 'test' as SITE_ID
INTO #PHAME_COHORT_COUNTS
FROM (            ------------<------------      Specify Site Name
    SELECT 'Total Unique Patients at Site' as Attribute, COUNT(DISTINCT PATID) n FROM dbo.DEMOGRAPHIC
    UNION ALL
    SELECT 'Total Unique Patients before Randomization' as Attribute, COUNT(DISTINCT CAP_ID) n FROM #study_population
    UNION ALL
    SELECT 'Total Unique Patients after Randomization' as Attribute, COUNT(DISTINCT CAP_ID) n FROM #randomly_stratified
    UNION ALL
    SELECT 'Female - Age betwn 18-30' as Attribute, COUNT(DISTINCT CAP_ID) n FROM #randomly_stratified WHERE SEX = 'F' AND AGE_CAT = '01'
    UNION ALL
    SELECT 'Male - Age betwn 18-30' as Attribute, COUNT(DISTINCT CAP_ID) n FROM #randomly_stratified WHERE SEX = 'M' AND AGE_CAT = '01'
    UNION ALL
    SELECT 'Female - Age betwn 31-40' as Attribute, COUNT(DISTINCT CAP_ID) n  FROM #randomly_stratified WHERE SEX = 'F' AND AGE_CAT = '02'
    UNION ALL
    SELECT 'Male - Age betwn 31-40' as Attribute, COUNT(DISTINCT CAP_ID) n  FROM #randomly_stratified WHERE SEX = 'M' AND AGE_CAT = '02'
    UNION ALL
    SELECT 'Female - Age betwn 41-50' as Attribute, COUNT(DISTINCT CAP_ID) n  FROM #randomly_stratified WHERE SEX = 'F' AND AGE_CAT = '03'
    UNION ALL
    SELECT 'Male - Age betwn 41-50' as Attribute, COUNT(DISTINCT CAP_ID) n  FROM #randomly_stratified WHERE SEX = 'M' AND AGE_CAT = '03'
    UNION ALL
    SELECT 'Female - Age betwn 51-60' as Attribute, COUNT(DISTINCT CAP_ID) n  FROM #randomly_stratified WHERE SEX = 'F' AND AGE_CAT = '04'
    UNION ALL
    SELECT 'Male - Age betwn 51-60' as Attribute, COUNT(DISTINCT CAP_ID) n FROM #randomly_stratified WHERE SEX = 'M' AND AGE_CAT = '04'
    UNION ALL
    SELECT 'Female - Age betwn 61-70' as Attribute, COUNT(DISTINCT CAP_ID) n  FROM #randomly_stratified WHERE SEX = 'F' AND AGE_CAT = '05'
    UNION ALL
    SELECT 'Male - Age betwn 61-70' as Attribute, COUNT(DISTINCT CAP_ID) n  FROM #randomly_stratified WHERE SEX = 'M' AND AGE_CAT = '05'
    UNION ALL
    SELECT 'Female - Age betwn >70' as Attribute, COUNT(DISTINCT CAP_ID) n  FROM #randomly_stratified WHERE SEX = 'F' AND AGE_CAT = '06'
    UNION ALL
    SELECT 'Male - Age betwn >70' as Attribute, COUNT(DISTINCT CAP_ID) n  FROM #randomly_stratified WHERE SEX = 'M' AND AGE_CAT = '06'
    ) a;  -- save output as PHAME_COHORT_COUNTS.csv
