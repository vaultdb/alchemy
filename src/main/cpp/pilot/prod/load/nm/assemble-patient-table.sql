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
 

\echo 'Setting up patient table!'
DROP TABLE IF EXISTS patient;

SELECT d.pat_id, study_year, age_days,  '0' as age_strata, -- placeholder 
       sex, ethnicity, race,
       numerator AND not denom_excl numerator,
       denom_excl, 1 as site_id
INTO patient
FROM demographics d JOIN population_labels p  ON d.pat_id = p.pat_id AND d.site_id = p.site_id AND d.study_year = p.study_year;




-- look for dupes:
-- SELECT pat_id, study_year, site_id FROM patient GROUP  BY pat_id, study_year, site_id HAVING COUNT(*) > 1 ORDER BY pat_id;


-- look at their values for inconsistencies:
-- enrich_htn_prod=# SELECT * FROM patient WHERE pat_id IN (174432, 175232, 208865, 243178, 333856, 588404) ORDER BY pat_id, study_year;
-- * the ones that are anomalies - delete them
-- update all of 208865 to have ethnicity the same -- coin flip to Y
-- update 588404 to have race = 05

\echo 'TODO: JMR: address thees conflicts and deduplicate them'
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
