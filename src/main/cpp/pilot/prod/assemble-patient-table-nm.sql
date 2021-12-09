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
