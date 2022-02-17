-- example execution:
-- psql enrich_htn_prod --csv -v col=age_strata -v selection='study_year=2018' < pilot/prod/load/local-enrich.sql

SELECT :col, sum(numerator::INT) numerator_cnt, SUM(denominator::INT) denominator_cnt,
       SUM(CASE WHEN numerator AND multisite THEN 1 ELSE 0 END) numerator_multisite_cnt,
       SUM(CASE WHEN denominator AND multisite THEN 1 ELSE 0 END) denominator_multisite_cnt
FROM patient
WHERE :selection AND NOT denom_excl
GROUP BY :col
ORDER BY :col;


