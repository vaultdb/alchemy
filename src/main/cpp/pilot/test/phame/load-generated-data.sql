-- run after ./bin/generate_phame_data_n_parties
DROP TABLE IF EXISTS phame_demographic;
CREATE TABLE phame_demographic (patid int,
                         age char(1),
                         gender char(1),
                         ethnicity char(1),
                         race char(1),
                            zip char(5),
                            payer_primary char(2),
                            payer_secondary char(2),
                            site_id int);


\copy phame_demographic FROM 'pilot/test/input/0/phame_demographic.csv' WITH DELIMITER ',';
\copy phame_demographic FROM 'pilot/test/input/1/phame_demographic.csv' WITH DELIMITER ',';
\copy phame_demographic FROM 'pilot/test/input/2/phame_demographic.csv' WITH DELIMITER ',';
\copy phame_demographic FROM 'pilot/test/input/3/phame_demographic.csv' WITH DELIMITER ',';


DROP TABLE IF EXISTS phame_diagnosis;
CREATE TABLE phame_diagnosis (
    patid int,
    dx_diabetes bool,
    dx_hypertension bool,
    dx_breast_cancer bool,
    dx_lung_cancer bool,
    dx_colorectal_cancer bool,
    dx_cervical_cancer bool,
    site_id int);

\copy phame_diagnosis FROM 'pilot/test/input/0/phame_diagnosis.csv' WITH DELIMITER ',';
\copy phame_diagnosis FROM 'pilot/test/input/1/phame_diagnosis.csv' WITH DELIMITER ',';
\copy phame_diagnosis FROM 'pilot/test/input/2/phame_diagnosis.csv' WITH DELIMITER ',';
\copy phame_diagnosis FROM 'pilot/test/input/3/phame_diagnosis.csv' WITH DELIMITER ',';
