WITH cohort AS (SELECT DISTINCT  pat_id, min(age_strata) age_strata, sex, ethnicity, race, numerator, denom_excl
                FROM patient
                GROUP BY pat_id, sex, ethnicity, race, numerator, denom_excl
                ORDER BY pat_id),
     aggs AS (SELECT age_strata, sex, ethnicity, race, SUM(CASE WHEN numerator AND NOT denom_excl THEN 1 ELSE 0 END)::INT numerator_cnt,
                                                       SUM(CASE WHEN NOT denom_excl THEN 1 ELSE 0 END)::INT denominator_cnt
                                  FROM cohort
                                  GROUP BY age_strata, sex, ethnicity, race)
SELECT d.*, COALESCE(numerator_cnt, 0) numerator_cnt, COALESCE(denominator_cnt, 0) denominator_cnt
FROM demographics_domain d LEFT JOIN aggs a on d.age_strata = a.age_strata  AND d.sex = a.sex  AND d.ethnicity = a.ethnicity AND d.race = a.race
ORDER BY age_strata, sex, ethnicity, race;