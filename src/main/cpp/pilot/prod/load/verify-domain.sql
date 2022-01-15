\set ECHO none
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

\echo 'Expect: '
\echo 'pat_id(int), study_year(smallint), age_strata(char(1)), sex(char(1)), ethnicity(char(1)), race(char(1)), numerator(bool), denominator_excl(bool), site_id(int)'

\d patient
