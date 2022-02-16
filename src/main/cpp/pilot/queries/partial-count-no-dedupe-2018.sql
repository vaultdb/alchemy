WITH aggs AS (SELECT age_strata, sex, ethnicity, race, SUM(CASE WHEN numerator AND NOT denom_excl THEN 1 ELSE 0 END)::INT numerator_cnt,
                                                       SUM(CASE WHEN ((NOT denom_excl) AND denominator) THEN 1 ELSE 0 END)::INT denominator_cnt
                                  FROM patient
                                  WHERE study_year = 2018
                                  GROUP BY age_strata, sex, ethnicity, race, site_id)
SELECT d.*, COALESCE(numerator_cnt, 0) numerator_cnt, COALESCE(denominator_cnt, 0) denominator_cnt
 FROM demographics_domain d LEFT JOIN aggs a on d.age_strata = a.age_strata  AND d.sex = a.sex  AND d.ethnicity = a.ethnicity AND d.race = a.race
 ORDER BY age_strata, sex, ethnicity, race;
