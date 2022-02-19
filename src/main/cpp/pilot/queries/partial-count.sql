WITH single_site AS (
    SELECT study_year, pat_id, age_strata, sex, ethnicity, race, numerator, denominator
    FROM patient
    WHERE NOT multisite AND NOT denom_excl AND :selection),
     full_domain AS (
         SELECT d.*, CASE WHEN p.numerator  THEN 1 ELSE 0 END numerator,
                CASE WHEN  p.denominator THEN 1 ELSE 0 END  denominator
         FROM demographics_domain d LEFT JOIN single_site p on d.study_year = p.study_year AND d.age_strata = p.age_strata  AND d.sex = p.sex  AND d.ethnicity = p.ethnicity AND d.race = p.race)
SELECT study_year, age_strata, sex, ethnicity, race, SUM(numerator)::INT numerator_cnt, sum(denominator)::INT denominator_cnt, 0 AS numerator_multisite_cnt, 0 AS denominator_multisite_cnt
FROM full_domain
GROUP BY study_year, age_strata, sex, ethnicity, race
ORDER BY study_year, age_strata, sex, ethnicity, race;
