-- based on: https://towardsdatascience.com/how-to-set-up-a-foreign-data-wrapper-in-postgresql-ebec152827f3
-- depends on:
-- \set target_db <db name>
-- also: CREATE EXTENSION  IF NOT EXISTS postgres_fdw;
\set target_db_str  '''':target_db''''


DROP SERVER IF EXISTS :target_db CASCADE;

CREATE SERVER :target_db FOREIGN DATA WRAPPER postgres_fdw OPTIONS (host 'localhost', dbname :target_db_str, port '5432');
CREATE USER MAPPING FOR vaultdb SERVER :target_db OPTIONS (user 'vaultdb');
GRANT USAGE  ON FOREIGN SERVER :target_db TO vaultdb;

DROP SCHEMA IF EXISTS :target_db;
CREATE SCHEMA :target_db;

IMPORT FOREIGN SCHEMA public  FROM SERVER :target_db INTO :target_db;


