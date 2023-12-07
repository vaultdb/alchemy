-- run after ./bin/generate_phame_data_three_parties pilot/test/input/phame 500
DROP TABLE IF EXISTS phame_demo;
CREATE TABLE phame_demo (patid int,
                         age float, -- [18, 100]
                         gender char(2),
                         race char(2),
                            ethnicity char(2),
                            zip char(5),
                            payer_primary char(2),
                            payer_secondary char(2),
                            site_id int);

\copy phame_demo(patid, age,gender, race, ethnicity, zip, payer_primary, payer_secondary, site_id) FROM 'pilot/test/input/phame/alice-patient.csv' WITH DELIMITER ',';
\copy phame_demo(patid, age,gender, race, ethnicity, zip, payer_primary, payer_secondary, site_id) FROM 'pilot/test/input/phame/bob-patient.csv' WITH DELIMITER ',';
\copy phame_demo(patid, age,gender, race, ethnicity, zip, payer_primary, payer_secondary, site_id) FROM 'pilot/test/input/phame/chi-patient.csv' WITH DELIMITER ',';

DROP TABLE IF EXISTS phame_diag_ins;
CREATE TABLE phame_diag_ins (
    patid int,
    diag_date date,
    dx_code char(8),
    dx_label char(3),
    payer_primary char(2),
    payer_secondary char(2));

\copy phame_diag_ins(patid, diag_date, dx_code, dx_label, payer_primary, payer_secondary) FROM 'pilot/test/input/phame/alice-dx.csv' WITH DELIMITER ',';
\copy phame_diag_ins(patid, diag_date, dx_code, dx_label, payer_primary, payer_secondary) FROM 'pilot/test/input/phame/bob-dx.csv' WITH DELIMITER ',';
\copy phame_diag_ins(patid, diag_date, dx_code, dx_label, payer_primary, payer_secondary) FROM 'pilot/test/input/phame/chi-dx.csv' WITH DELIMITER ',';