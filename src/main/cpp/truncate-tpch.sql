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


-- now do alice and bob
\set party_db 'tpch_alice_':cutoff
DROP DATABASE IF EXISTS :party_db;
CREATE DATABASE :party_db  WITH TEMPLATE :target_db;
\c :party_db
\i 'select-alice.sql'
SELECT split_tables_alice();


\set party_db 'tpch_bob_':cutoff
DROP DATABASE IF EXISTS :party_db;
CREATE DATABASE :party_db  WITH TEMPLATE :target_db;
\c :party_db
\i 'select-bob.sql'
SELECT split_tables_bob();
