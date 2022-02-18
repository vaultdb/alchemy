-- example execution:
-- psql enrich_htn_prod --csv -v col=age_strata  < pilot/queries/local-enrich.sql

\set quote ''''

\set col age_strata
\set col_name :quote:col:quote

DROP TABLE IF EXISTS distinct_patient;
CREATE TEMP TABLE distinct_patient AS (
       SELECT DISTINCT site_id, pat_id, age_strata, sex, ethnicity, race, numerator, denominator, denom_excl, multisite
       FROM patient);
       
SELECT  site_id, :col_name stratification, :col, sum(numerator::INT) numerator_cnt, SUM(denominator::INT) denominator_cnt,
       SUM(CASE WHEN numerator AND multisite THEN 1 ELSE 0 END) numerator_multisite_cnt,
       SUM(CASE WHEN denominator AND multisite THEN 1 ELSE 0 END) denominator_multisite_cnt
FROM distinct_patient
WHERE NOT denom_excl
GROUP BY  site_id, :col
ORDER BY  site_id, :col;



\set col sex
\set col_name :quote:col:quote

SELECT site_id, :col_name stratification, :col, sum(numerator::INT) numerator_cnt, SUM(denominator::INT) denominator_cnt,
       SUM(CASE WHEN numerator AND multisite THEN 1 ELSE 0 END) numerator_multisite_cnt,
       SUM(CASE WHEN denominator AND multisite THEN 1 ELSE 0 END) denominator_multisite_cnt
FROM  distinct_patient
WHERE NOT denom_excl
GROUP BY  site_id, :col
ORDER BY  site_id, :col;



\set col ethnicity
\set col_name :quote:col:quote

SELECT site_id, :col_name stratification, :col, sum(numerator::INT) numerator_cnt, SUM(denominator::INT) denominator_cnt,
       SUM(CASE WHEN numerator AND multisite THEN 1 ELSE 0 END) numerator_multisite_cnt,
       SUM(CASE WHEN denominator AND multisite THEN 1 ELSE 0 END) denominator_multisite_cnt
FROM  distinct_patient
WHERE NOT denom_excl
GROUP BY  site_id, :col
ORDER BY  site_id, :col;



\set col race
\set col_name :quote:col:quote

SELECT site_id, :col_name stratification, :col, sum(numerator::INT) numerator_cnt, SUM(denominator::INT) denominator_cnt,
       SUM(CASE WHEN numerator AND multisite THEN 1 ELSE 0 END) numerator_multisite_cnt,
       SUM(CASE WHEN denominator AND multisite THEN 1 ELSE 0 END) denominator_multisite_cnt
FROM  distinct_patient
WHERE NOT denom_excl
GROUP BY  site_id, :col
ORDER BY  site_id, :col;


