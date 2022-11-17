-- relies on cutoff being initialized
-- all other tables delete corresponding tuples with cascade
\set ECHO all
DROP FUNCTION IF EXISTS truncate_tables;
CREATE OR REPLACE FUNCTION truncate_tables(cutoff integer) RETURNS VOID AS
$BODY$
BEGIN
-- prevent it from deleting parts and suppliers that appear in more than one order
EXECUTE 'ALTER TABLE lineitem DROP CONSTRAINT lineitem_l_partkey_l_suppkey_fkey';

-- cascades to orders, lineitem
RAISE NOTICE 'Deleting rows from customer, orders, and lineitem';
EXECUTE 'DELETE FROM customer WHERE c_custkey > ' || cutoff;

RAISE NOTICE 'Deleting rows from partsupp';

EXECUTE 'WITH to_delete AS (
    (SELECT ps_partkey, ps_suppkey FROM partsupp)
        EXCEPT
        (SELECT DISTINCT l_partkey, l_suppkey FROM lineitem))
DELETE FROM partsupp
    WHERE (ps_partkey, ps_suppkey) IN (SELECT * FROM to_delete)';

RAISE NOTICE 'Deleting rows from supplier';

EXECUTE 'WITH to_delete AS (
    (SELECT s_suppkey FROM supplier)
        EXCEPT
        (SELECT DISTINCT ps_suppkey FROM partsupp))
DELETE FROM supplier
    WHERE s_suppkey IN (SELECT * FROM to_delete)';


RAISE NOTICE 'Deleting rows from part';

EXECUTE 'WITH to_delete AS (
    (SELECT p_partkey FROM part)
        EXCEPT
        (SELECT DISTINCT ps_partkey FROM partsupp))
DELETE FROM part
    WHERE p_partkey IN (SELECT * FROM to_delete)';


EXECUTE 'ALTER TABLE lineitem ADD CONSTRAINT lineitem_l_partkey_l_suppkey_fkey FOREIGN KEY (l_partkey , l_suppkey) ' ||
'REFERENCES partsupp(ps_partkey, ps_suppkey)';

END
$BODY$
LANGUAGE plpgsql;


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
SELECT truncate_tables(:cutoff);


-- now do alice and bob
\set target_db 'tpch_alice_':cutoff
DROP DATABASE IF EXISTS :target_db;
CREATE DATABASE :target_db  WITH TEMPLATE tpch_alice;
\c :target_db
SELECT truncate_tables(:cutoff);



\set target_db 'tpch_bob_':cutoff
DROP DATABASE IF EXISTS :target_db;
CREATE DATABASE :target_db  WITH TEMPLATE tpch_bob;
\c :target_db
SELECT truncate_tables(:cutoff);
