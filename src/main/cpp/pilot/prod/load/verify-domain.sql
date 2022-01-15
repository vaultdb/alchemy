
\set col age_strata

\echo 'Expect 1-7 stratum for age'
SELECT :col, COUNT(*)
FROM patient
GROUP BY :col
ORDER BY :col;


\set col sex
\echo 'Expect domain M, F, U for sex'
SELECT :col, COUNT(*)
FROM patient
GROUP BY :col
ORDER BY :col;


\set col ethnicity
\echo 'Expect Y, N, U for ethnicity'
SELECT :col, COUNT(*)
FROM patient
GROUP BY :col
ORDER BY :col;



\set col race
\echo 'Expect domain 1-7 for race'
SELECT :col, COUNT(*)
FROM patient
GROUP BY :col
ORDER BY :col;
