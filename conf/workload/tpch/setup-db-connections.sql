-- run this as vaultdb role in postgresql
-- e.g.
-- psql -U vaultdb postgres < conf/workload/tpch/setup-db-connections.sql
CREATE EXTENSION  IF NOT EXISTS postgres_fdw;
-- generate this with:
-- OUTPUT=t.sql
-- rm $OUTPUT
--   for db in $(psql postgres -tU vaultdb -c "SELECT datname  FROM pg_database WHERE datname LIKE 'tpch%' ORDER BY datname");  do  echo '\set target_db ' $db >> $OUTPUT;  echo \\i \'conf/workload/tpch/create-fdw-connection.sql\' >> $OUTPUT;  done

\set target_db  tpch_alice
\i 'conf/workload/tpch/create-fdw-connection.sql'
\set target_db  tpch_alice_150
\i 'conf/workload/tpch/create-fdw-connection.sql'
\set target_db  tpch_alice_1500
\i 'conf/workload/tpch/create-fdw-connection.sql'
\set target_db  tpch_alice_15000
\i 'conf/workload/tpch/create-fdw-connection.sql'
\set target_db  tpch_alice_30000
\i 'conf/workload/tpch/create-fdw-connection.sql'
\set target_db  tpch_alice_60000
\i 'conf/workload/tpch/create-fdw-connection.sql'
\set target_db  tpch_bob
\i 'conf/workload/tpch/create-fdw-connection.sql'
\set target_db  tpch_bob_150
\i 'conf/workload/tpch/create-fdw-connection.sql'
\set target_db  tpch_bob_1500
\i 'conf/workload/tpch/create-fdw-connection.sql'
\set target_db  tpch_bob_15000
\i 'conf/workload/tpch/create-fdw-connection.sql'
\set target_db  tpch_bob_30000
\i 'conf/workload/tpch/create-fdw-connection.sql'
\set target_db  tpch_bob_60000
\i 'conf/workload/tpch/create-fdw-connection.sql'
\set target_db  tpch_unioned
\i 'conf/workload/tpch/create-fdw-connection.sql'
\set target_db  tpch_unioned_150
\i 'conf/workload/tpch/create-fdw-connection.sql'
\set target_db  tpch_unioned_1500
\i 'conf/workload/tpch/create-fdw-connection.sql'
\set target_db  tpch_unioned_15000
\i 'conf/workload/tpch/create-fdw-connection.sql'
\set target_db  tpch_unioned_30000
\i 'conf/workload/tpch/create-fdw-connection.sql'
\set target_db  tpch_unioned_60000
\i 'conf/workload/tpch/create-fdw-connection.sql'
