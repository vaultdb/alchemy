-- for chi site only
WITH labeled as (
        SELECT pat_id, age_strata, sex, ethnicity, race, numerator, denom_excl
        FROM patient
	WHERE site_id = 3
        ORDER BY pat_id),
  deduplicated AS (    SELECT p.pat_id,  age_strata, sex, ethnicity, race, MAX(p.numerator::INT) numerator, COUNT(*) cnt
    FROM labeled p
    GROUP BY p.pat_id, age_strata, sex, ethnicity, race
    HAVING MAX(denom_excl::INT) = 0
    ORDER BY p.pat_id, age_strata, sex, ethnicity, race ), 
  data_cube AS (SELECT  age_strata, sex, ethnicity, race, SUM(numerator)::INT numerator_cnt, COUNT(*)::INT denominator_cnt,
                       SUM(CASE WHEN (numerator > 0 AND cnt> 1) THEN 1 ELSE 0 END)::INT numerator_multisite_cnt, SUM(CASE WHEN (cnt > 1) THEN 1 else 0 END)::INT  denominator_multisite_cnt
  FROM deduplicated
  GROUP BY  age_strata, sex, ethnicity, race
  ORDER BY  age_strata, sex, ethnicity, race)
SELECT d.*, COALESCE(numerator_cnt, 0) numerator_cnt, COALESCE(denominator_cnt, 0) denominator_cnt, COALESCE(numerator_multisite_cnt, 0) numerator_multisite_cnt, COALESCE(denominator_multisite_cnt, 0) denominator_multisite_cnt
FROM demographics_domain d LEFT JOIN data_cube p on d.age_strata = p.age_strata  AND d.sex = p.sex  AND d.ethnicity = p.ethnicity AND d.race = p.race ORDER BY d.age_strata, d.sex, d.ethnicity, d.race;
