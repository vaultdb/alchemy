-- example execution:
-- psql enrich_htn_prod --csv -v col=age_strata  < pilot/queries/local-enrich.sql

SELECT study_year, :col, sum(numerator::INT) numerator_cnt, SUM(denominator::INT) denominator_cnt,
       SUM(CASE WHEN numerator AND multisite THEN 1 ELSE 0 END) numerator_multisite_cnt,
       SUM(CASE WHEN denominator AND multisite THEN 1 ELSE 0 END) denominator_multisite_cnt
FROM patient
WHERE NOT denom_excl
GROUP BY  study_year, :col
ORDER BY  study_year, :col;


