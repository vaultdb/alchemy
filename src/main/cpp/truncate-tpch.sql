-- relies on cutoff being initialized
-- all other tables delete corresponding tuples with cascade
\set target_db 'tpch_unioned_':cutoff

DROP DATABASE IF EXISTS :target_db;
CREATE DATABASE :target_db  WITH TEMPLATE tpch_unioned;
\c :target_db


ALTER TABLE lineitem DROP CONSTRAINT lineitem_l_partkey_l_suppkey_fkey;
DELETE FROM customer WHERE c_custkey > :cutoff;
DELETE FROM supplier WHERE s_suppkey > :cutoff;


-- now do alice and bob
-- \set target_db 'tpch_alice_':cutoff
-- DROP DATABASE IF EXISTS :target_db;
-- CREATE DATABASE :target_db  WITH TEMPLATE tpch_alice;
-- \c :target_db
-- 
-- 
-- DELETE FROM customer WHERE c_custkey > :cutoff;
-- DELETE FROM supplier WHERE s_suppkey > :cutoff;
-- 
-- 
-- \set target_db 'tpch_bob_':cutoff
-- DROP DATABASE IF EXISTS :target_db;
-- CREATE DATABASE :target_db  WITH TEMPLATE tpch_bob;
-- \c :target_db
-- 
-- 
-- DELETE FROM customer WHERE c_custkey > :cutoff;
-- DELETE FROM supplier WHERE s_suppkey > :cutoff;
-- 
