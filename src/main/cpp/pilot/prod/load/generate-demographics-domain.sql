DROP TABLE IF EXISTS age_strata_domain;
CREATE TEMP TABLE age_strata_domain(
    age_strata char(1));
INSERT INTO age_strata_domain
    VALUES ('1'), ('2'), ('3'), ('4'), ('5'), ('6'), ('7');

DROP TABLE IF EXISTS  sex_domain;
CREATE TEMP TABLE sex_domain(
    sex char(1));

INSERT INTO sex_domain VALUES ('F'), ('M'), ('U');

DROP TABLE IF EXISTS  ethnicity_domain;
CREATE TEMP TABLE ethnicity_domain(
    ethnicity char(1));

INSERT INTO ethnicity_domain VALUES ('N'), ('U'), ('Y');

DROP TABLE IF EXISTS race_domain;
CREATE TEMP TABLE race_domain(
    race char(1));
INSERT INTO race_domain
    VALUES ('1'), ('2'), ('3'), ('4'), ('5'), ('6'), ('7');

DROP TABLE IF EXISTS demographics_domain;

-- all pairs for oblivious padding
SELECT age_strata, sex, ethnicity, race
INTO demographics_domain
FROM age_strata_domain, sex_domain, ethnicity_domain, race_domain;
