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
   ORDER BY p.study_year, p.pat_id, age_strata, sex, ethnicity, race)

SELECT  study_year, age_strata, sex, ethnicity, race,
	         SUM(numerator)::INT  numerator_cnt,  SUM(denominator)::INT denominator_cnt,
                 SUM(CASE WHEN (numerator > 0 AND cnt> 1) THEN 1 ELSE 0 END)::INT numerator_multisite_cnt,
	         SUM(CASE WHEN (denominator > 0 AND cnt > 1) THEN 1 else 0 END)::INT  denominator_multisite_cnt
FROM deduplicated
GROUP BY  study_year, age_strata, sex, ethnicity, race
ORDER BY  study_year, age_strata, sex, ethnicity, race;
