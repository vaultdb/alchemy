--   set up tpch_zk_bob to make schema available to verifier in ZKSQL
-- needed for zk_base_test.*

\set target_db tpch_zk_bob
DROP DATABASE IF EXISTS :target_db;
CREATE DATABASE :target_db  WITH TEMPLATE tpch_unioned_int; -- or insert your source DB here
\c :target_db
DELETE FROM lineitem CASCADE;
DELETE FROM orders CASCADE;
DELETE FROM customer CASCADE;
DELETE FROM supplier CASCADE;
DELETE FROM region CASCADE;
DELETE FROM nation CASCADE;
DELETE FROM part CASCADE;
