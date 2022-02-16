WITH single_site AS (
    SELECT pat_id, age_strata, sex, ethnicity, race, numerator, denominator, denom_excl
    FROM patient
    WHERE NOT multisite AND NOT denom_excl),
     full_domain AS (
         SELECT d.*, CASE WHEN p.numerator  THEN 1 ELSE 0 END numerator,
                CASE WHEN p.denominator  THEN 1 ELSE 0 END  denominator
         FROM demographics_domain d LEFT JOIN single_site p on d.age_strata = p.age_strata  AND d.sex = p.sex  AND d.ethnicity = p.ethnicity AND d.race = p.race)
SELECT age_strata, sex, ethnicity, race, SUM(numerator)::INT numerator_cnt, sum(denominator)::INT denominator_cnt, 0 AS numerator_multisite_cnt, 0 AS denominator_multisite_cnt
FROM full_domain
GROUP BY age_strata, sex, ethnicity, race
ORDER BY age_strata, sex, ethnicity, race;
