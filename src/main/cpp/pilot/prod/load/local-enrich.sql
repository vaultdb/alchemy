\set col age_strata

SELECT :col, SUM(CASE WHEN numerator THEN 1 else 0 END) numerator_cnt, SUM(CASE WHEN denominator THEN 1 ELSE 0 END) denom_cnt
FROM patient
GROUP BY :col
ORDER BY :col;
