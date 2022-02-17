DROP TABLE IF EXISTS local_aggregates;

-- follows running pilot/queries/local_enrich.sql on all hosts
CREATE TABLE local_aggregates (
       study_year smallint,
       site_id smallint,
       stratification varchar,
       study_var varchar,
       numerator_cnt int,
       denominator_cnt int,
       numerator_multisite_cnt int,
       denominator_multisite_cnt int);

\copy local_aggregates FROM 'pilot/results/nm-local-aggregates.csv' CSV
\copy local_aggregates FROM 'pilot/results/rush-local-aggregates.csv' CSV
\copy local_aggregates FROM 'pilot/results/alliance-local-aggregates.csv' CSV


UPDATE local_aggregates SET study_var='Age (18-28)' WHERE stratification='age_strata' AND study_var='1';
UPDATE local_aggregates SET study_var='Age (29-39)' WHERE stratification='age_strata' AND study_var='2';
UPDATE local_aggregates SET study_var='Age (40-50)' WHERE stratification='age_strata' AND study_var='3';
UPDATE local_aggregates SET study_var='Age (51-61)' WHERE stratification='age_strata' AND study_var='4';
UPDATE local_aggregates SET study_var='Age (62-72)' WHERE stratification='age_strata' AND study_var='5';
UPDATE local_aggregates SET study_var='Age (73-83)' WHERE stratification='age_strata' AND study_var='6';
UPDATE local_aggregates SET study_var='Age (84-100)' WHERE stratification='age_strata' AND study_var='7';

UPDATE local_aggregates SET study_var='Male' WHERE stratification='sex' AND study_var='M';
UPDATE local_aggregates SET study_var='Female' WHERE stratification='sex' AND study_var='F';
UPDATE local_aggregates SET study_var='Unknown or Non-Binary' WHERE stratification='sex' AND study_var='U';

UPDATE local_aggregates SET study_var='Hispanic' WHERE stratification='ethnicity' AND study_var='Y';
UPDATE local_aggregates SET study_var='Non-Hispanic' WHERE stratification='ethnicity' AND study_var='N';
UPDATE local_aggregates SET study_var='Unknown' WHERE stratification='ethnicity' AND study_var='U';

UPDATE local_aggregates SET study_var='American Indian or Alaska Native (1)' WHERE stratification='race' AND study_var='1';
UPDATE local_aggregates SET study_var='Asian (2)' WHERE stratification='race' AND study_var='2';
UPDATE local_aggregates SET study_var='Black or African American (3)' WHERE stratification='race' AND study_var='3';
UPDATE local_aggregates SET study_var='Native Hawaiian or Other Pacific Islander (4)' WHERE stratification='race' AND study_var='4';
UPDATE local_aggregates SET study_var='White (5)' WHERE stratification='race' AND study_var='5';
UPDATE local_aggregates SET study_var='Multiracial (6)' WHERE stratification='race' AND study_var='6';
UPDATE local_aggregates SET study_var='Unknown (7)' WHERE stratification='race' AND study_var='7';



