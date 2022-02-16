DROP TABLE IF EXISTS patient;
CREATE TABLE patient (
       pat_id int,
       study_year int,
       age_strata char(1),
       sex  char(1),
       ethnicity  char(1),
       race  char(1),
       numerator bool default null,
       denominator bool default null,
       denom_excl bool default null,
       site_id int,
       multisite bool default false,
       hash int);
