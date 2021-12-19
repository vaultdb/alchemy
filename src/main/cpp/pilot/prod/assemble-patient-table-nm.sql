-- to be run after load-demographics-nm.sql and load-population-labels-nm.sql  

\echo 'Setting up patient table!'
DROP TABLE IF EXISTS patient;


-- planned table:
--   CREATE TABLE patient (                                                                                                                                                                                                                                  
--      pat_id int,
--      study_year smallint, -- new addition, not in testing
--      zip_marker varchar(3),                                                                                                                                                                                                                               
--      age_days integer,                                                                                                                                                                                                                                    
--     sex varchar(2), -- was varchar(1) in testing
--     ethnicity varchar(2), -- was bool in testing
--     race int, -- TODO: convert race to int?  Maybe later, if there's an issue.  But doing so will be lossy if we don't rejigger the mapping of IDs to values
--     numerator bool default null, -- numerator group membership                                                                                                                                                                                            
--     denominator bool default null, -- denominator group membership                                                                                                                                                                                        
--     denom_excl int default null, -- excluded from study
--     site_id int);                                                                                                                                                                                                                                        
 



SELECT d.pat_id, d.study_year, zip_marker, age_days, sex, ethnicity, race, d.numerator demographic_numerator, p.numerator population_numerator, denom_excl denom_excl_original, d.site_id site_id_str
INTO patient
FROM demographics d JOIN population_labels p  ON d.pat_id = p.pat_id AND d.study_year = p.study_year AND d.site_id = p.site_id;


-- add new cols in the correct order - shuffling the order of denom_excl to make debugging easier
ALTER TABLE patient ADD COLUMN numerator bool default false;
ALTER TABLE patient ADD COLUMN denominator bool default true; -- everyone is in the denom unless otherwise noted
ALTER TABLE patient ADD COLUMN denom_excl bool default false;
ALTER TABLE patient ADD COLUMN site_id INT;

UPDATE patient SET denom_excl=denom_excl_original;
ALTER TABLE patient DROP COLUMN denom_excl_original;

UPDATE patient SET numerator=true WHERE demographic_numerator AND NOT denom_excl;
UPDATE patient SET denominator=false WHERE denom_excl OR  population_numerator;
ALTER TABLE patient DROP COLUMN demographic_numerator;
ALTER TABLE patient DROP COLUMN population_numerator;

-- site_id = (1, NM) (2, RU) (3, AC)
UPDATE patient SET site_id=1 WHERE site_id_str='NW';
UPDATE patient SET site_id=2 WHERE site_id_str='RU';
UPDATE patient SET site_id=3 WHERE site_id_str='AC';
ALTER TABLE patient DROP COLUMN site_id_str;

-- look for dupes:
-- SELECT pat_id, study_year, site_id FROM patient GROUP  BY pat_id, study_year, site_id HAVING COUNT(*) > 1 ORDER BY pat_id;
-- output: (174432, 175232, 208865, 243178, 333856, 588404)

-- look at their values for inconsistencies:
-- enrich_htn_prod=# SELECT * FROM patient WHERE pat_id IN (174432, 175232, 208865, 243178, 333856, 588404) ORDER BY pat_id, study_year;
--  pat_id | study_year | zip_marker | age_days | sex | ethnicity | race | numerator | denominator | denom_excl | site_id 
-- --------+------------+------------+----------+-----+-----------+------+-----------+-------------+------------+---------
-- *174432 |       2018 | 000        |    25442 | F   | N         | 05   | f         | t           | f          |       1
--  174432 |       2018 | 000        |    25442 | F   | N         | 03   | f         | t           | f          |       1
--  174432 |       2019 | 000        |    25807 | F   | N         | 03   | f         | t           | f          |       1
--  174432 |       2020 | 000        |    26173 | F   | N         | 03   | f         | t           | f          |       1

--  175232 |       2019 | 000        |    23015 | F   | N         | 02   | f         | t           | f          |       1
-- *175232 |       2019 | 000        |    23015 | F   | N         | OT   | f         | t           | f          |       1

--  208865 |       2018 | 000        |    19439 | F   | N         | 05   | f         | t           | f          |       1
--  208865 |       2019 | 000        |    19804 | F   | Y         | 05   | f         | t           | f          |       1
-- *208865 |       2019 | 000        |    19804 | F   | N         | 05   | f         | t           | f          |       1
--  208865 |       2020 | 000        |    20170 | F   | Y         | 05   | f         | t           | f          |       1

--  243178 |       2018 | 000        |    24317 | M   | R         | 03   | f         | t           | f          |       1
--  243178 |       2019 | 000        |    24682 | M   | R         | 03   | f         | t           | f          |       1
-- *243178 |       2019 | 000        |    24682 | M   | R         | OT   | f         | t           | f          |       1
--  243178 |       2020 | 000        |    25048 | M   | R         | 03   | f         | t           | f          |       1
-- *243178 |       2020 | 000        |    25048 | M   | R         | OT   | f         | t           | f          |       1

--  333856 |       2019 | 000        |    19713 | F   | N         | 03   | f         | t           | f          |       1
-- *333856 |       2019 | 000        |    19713 | F   | N         | OT   | f         | t           | f          |       1

--  588404 |       2018 | 000        |     9581 | M   | N         | OT   | t         | f           | f          |       1
-- *588404 |       2019 | 000        |     9946 | M   | N         | OT   | t         | f           | f          |       1
--  588404 |       2019 | 000        |     9946 | M   | N         | 05   | t         | f           | f          |       1
-- (20 rows)
-- * the ones that are anomalies - delete them
-- update all of 208865 to have ethnicity the same -- coin flip to Y
-- update 588404 to have race = 05

DELETE FROM patient WHERE pat_id=174432 AND race='05';
DELETE FROM patient WHERE pat_id=175232 AND race='OT';
DELETE FROM patient WHERE pat_id=208865 AND study_year=2019 AND ethnicity='N';
UPDATE patient SET ethnicity='Y' WHERE pat_id=208865;
DELETE FROM patient WHERE pat_id=243178 AND race='OT';
DELETE FROM patient WHERE pat_id=333856 AND race='OT';
DELETE FROM patient WHERE pat_id=588404 AND study_year=2019 AND race='OT';
UPDATE patient SET race='05' WHERE pat_id=588404;





-- at the end we have:
-- > \d patient
--                        Table "public.patient"
--    Column    |         Type         | Collation | Nullable | Default 
-- -------------+----------------------+-----------+----------+---------
--  pat_id      | integer              |           |          | 
--  study_year  | integer              |           |          | 
--  zip_marker  | character varying(3) |           |          | 
--  age_days    | integer              |           |          | 
--  sex         | character varying(2) |           |          | 
--  ethnicity   | character varying(2) |           |          | 
--  race        | character varying(2) |           |          | 
--  numerator   | boolean              |           |          | false
--  denominator | boolean              |           |          | true
--  denom_excl  | boolean              |           |          | false
--  site_id     | integer              |           |          | 
-- 
