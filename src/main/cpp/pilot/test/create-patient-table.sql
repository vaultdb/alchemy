CREATE OR REPLACE FUNCTION setup_patient_table() RETURNS void  AS
$BODY$
BEGIN
    EXECUTE 'DROP TABLE IF EXISTS patient';
    EXECUTE 'CREATE TABLE patient (pat_id int, study_year int, age_strata char(1), sex  char(1), ethnicity  char(1),  race  char(1),  numerator bool default null, denom_excl bool default null, site_id int,  multisite bool default false)';
END
$BODY$
LANGUAGE plpgsql;

