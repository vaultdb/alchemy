WITH cohort AS (SELECT study_year, site_id, pat_id, age_strata, sex, ethnicity, race,  max(numerator::INT) numerator, max(denominator::INT) denominator
                FROM patient
		WHERE NOT denom_excl AND :selection
                GROUP BY study_year, site_id, pat_id, age_strata, sex, ethnicity, race
                ORDER BY study_year, pat_id),
     aggs AS (SELECT study_year, age_strata, sex, ethnicity, race, SUM(CASE WHEN numerator > 0  THEN 1 ELSE 0 END)::INT numerator_cnt,
                                                       SUM(CASE WHEN denominator > 0 THEN 1 ELSE 0 END)::INT denominator_cnt
                                  FROM cohort
                                  GROUP BY study_year, age_strata, sex, ethnicity, race)
SELECT d.*, COALESCE(numerator_cnt, 0) numerator_cnt, COALESCE(denominator_cnt, 0) denominator_cnt
FROM  (SELECT * FROM demographics_domain WHERE :selection)  d LEFT JOIN aggs a on d.age_strata = a.age_strata  AND d.sex = a.sex  AND d.ethnicity = a.ethnicity AND d.race = a.race AND a.study_year = d.study_year
ORDER BY study_year, age_strata, sex, ethnicity, race;