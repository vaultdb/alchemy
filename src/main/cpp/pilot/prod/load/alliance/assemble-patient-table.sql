-- to be run after load-demographics-nm.sql and load-population-labels-nm.sql  

\echo 'Setting up patient table!'
DROP TABLE IF EXISTS patient;


-- planned table:
--   CREATE TABLE patient (                                                                                --      pat_id int,
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
 



SELECT d.pat_id, study_year, 0 as age_days, -- placeholder
       age_years_2015, sex, ethnicity, race,
       numerator AND not denom_excl numerator,
       denom_excl, 3 as site_id
INTO patient
FROM demographics d JOIN population_labels p  ON d.pat_id = p.pat_id AND d.site_id = p.site_id;


-- populate age_days
UPDATE patient
SET age_days = (age_years_2015 + (study_year - 2015)) * 365;


-- look for dupes:
-- SELECT pat_id, study_year, site_id FROM patient GROUP  BY pat_id, study_year, site_id HAVING COUNT(*) > 1 ORDER BY pat_id;




-- at the end we have:
-- > \d patient
--                        Table "public.patient"
--    Column    |         Type         | Collation | Nullable | Default 
-- -------------+----------------------+-----------+----------+---------
--  pat_id      | integer              |           |          | 
--  study_year  | integer              |           |          | 
--  age_days    | integer              |           |          | 
--  sex         | character varying(2) |           |          | 
--  ethnicity   | character varying(2) |           |          | 
--  race        | character varying(2) |           |          | 
--  numerator   | boolean              |           |          | false
--  denom_excl  | boolean              |           |          | false
--  site_id     | integer              |           |          | 
-- 
