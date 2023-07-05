-- relies on cutoff being initialized
-- all other tables delete corresponding tuples with cascade
\set ECHO all


-- set default cutoff of 1000
\set cutoff :cutoff

-- use gset to initialize to 1k if it is set to ':cutoff', the placeholder set above
SELECT CASE
  WHEN :'cutoff'= ':cutoff'
  THEN '1000'
  ELSE :'cutoff'
END::numeric AS "cutoff";

\gset

\set target_db 'tpch_unioned_':cutoff



DROP DATABASE IF EXISTS :target_db;
CREATE DATABASE :target_db  WITH TEMPLATE tpch_unioned;
\c :target_db
\i 'truncate-limit-tpch-tables.sql'
SELECT truncate_tables(:cutoff);
\i 'set-constraints.sql'


-- now do alice and bob
\set target_db 'tpch_alice_':cutoff
DROP DATABASE IF EXISTS :target_db;
CREATE DATABASE :target_db  WITH TEMPLATE tpch_alice;
\c :target_db
\i 'truncate-limit-tpch-tables.sql'
SELECT truncate_tables(:cutoff);
\i 'set-constraints.sql'


\set target_db 'tpch_bob_':cutoff
DROP DATABASE IF EXISTS :target_db;
CREATE DATABASE :target_db  WITH TEMPLATE tpch_bob;
\c :target_db
\i 'truncate-limit-tpch-tables.sql'
SELECT truncate_tables(:cutoff);
\i 'set-constraints.sql'
