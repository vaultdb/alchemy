 WITH labeled as (
               SELECT  study_year, pat_id, age_strata, sex, ethnicity, race, numerator, denominator, denom_excl
               FROM  (SELECT DISTINCT study_year, pat_id, age_strata, sex, ethnicity, race, numerator, denominator, denom_excl, site_id FROM patient) p
               WHERE :selection
               ORDER BY study_year, pat_id),
  deduplicated AS (
   SELECT p.study_year, p.pat_id,  age_strata, sex, ethnicity, race,
         MAX(p.numerator::INT) numerator, MAX(p.denominator::INT) denominator, COUNT(*) cnt
   FROM labeled p
   GROUP BY p.study_year, p.pat_id, age_strata, sex, ethnicity, race
   HAVING MAX(denom_excl::INT) = 0
   ORDER BY p.study_year, p.pat_id, age_strata, sex, ethnicity, race),
data_cube AS(
             SELECT  study_year, age_strata, sex, ethnicity, race,
	         SUM(numerator)::INT  numerator_cnt,  SUM(denominator)::INT denominator_cnt,
                 SUM(CASE WHEN (numerator > 0 AND cnt> 1) THEN 1 ELSE 0 END)::INT numerator_multisite_cnt,
	         SUM(CASE WHEN (denominator > 0 AND cnt > 1) THEN 1 else 0 END)::INT  denominator_multisite_cnt
             FROM deduplicated
             GROUP BY  study_year, age_strata, sex, ethnicity, race
             ORDER BY  study_year, age_strata, sex, ethnicity, race)

SELECT d.*, COALESCE(numerator_cnt, 0) numerator_cnt, COALESCE(denominator_cnt, 0) denominator_cnt,
       COALESCE(numerator_multisite_cnt, 0) numerator_multisite_cnt,
       COALESCE(denominator_multisite_cnt, 0) denominator_multisite_cnt 
FROM (SELECT * FROM demographics_domain WHERE :selection) d LEFT JOIN data_cube p ON
     d.study_year = p.study_year AND d.age_strata = p.age_strata  AND d.sex = p.sex
     AND d.ethnicity = p.ethnicity AND d.race = p.race 
ORDER BY d.study_year, d.age_strata, d.sex, d.ethnicity, d.race;

