WITH dist_patients AS (
SELECT DISTINCT pat_id,  max(denominator::INT) denominator, max(numerator::INT) numerator, age_strata, sex, ethnicity, race
     FROM patient
     WHERE NOT denom_excl AND multisite
     GROUP BY pat_id, age_strata, sex, ethnicity, race)
SELECT :col, SUM(numerator) numerator_cnt, SUM(denominator) denom_cnt
FROM dist_patients
GROUP BY :col
ORDER BY :col;
