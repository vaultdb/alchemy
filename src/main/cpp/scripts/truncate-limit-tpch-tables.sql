CREATE FUNCTION truncate_tables(cutoff integer) RETURNS VOID AS
    $BODY$
DECLARE
max_key INTEGER;
BEGIN

  -- Disable triggers
  SET session_replication_role = replica;

  RAISE NOTICE 'customer';
SELECT c_custkey INTO max_key FROM customer ORDER BY c_custkey LIMIT 1 OFFSET cutoff;
EXECUTE 'DELETE FROM customer WHERE c_custkey >= ' || max_key;

RAISE NOTICE 'supplier';
SELECT s_suppkey INTO max_key FROM supplier ORDER BY s_suppkey LIMIT 1 OFFSET cutoff / 15;
EXECUTE 'DELETE FROM supplier WHERE s_suppkey >= ' || max_key;

RAISE NOTICE 'part';
SELECT p_partkey INTO max_key FROM part ORDER BY p_partkey LIMIT 1 OFFSET cutoff * 4 / 3;
EXECUTE 'DELETE FROM part WHERE p_partkey >= ' || max_key;

RAISE NOTICE 'orders';
SELECT o_orderkey INTO max_key FROM orders ORDER BY o_orderkey LIMIT 1 OFFSET cutoff * 10;
EXECUTE 'DELETE FROM orders WHERE o_orderkey >= ' || max_key;

RAISE NOTICE 'lineitem';
SELECT l_orderkey INTO max_key FROM lineitem ORDER BY l_orderkey, l_linenumber LIMIT 1 OFFSET cutoff * 40;
EXECUTE 'DELETE FROM lineitem WHERE l_orderkey >= ' || max_key;

RAISE NOTICE 'partsupp';
SELECT ps_partkey INTO max_key FROM partsupp ORDER BY ps_partkey, ps_suppkey LIMIT 1 OFFSET cutoff * 16 / 3;
EXECUTE 'DELETE FROM partsupp WHERE ps_partkey >= ' || max_key;

-- Re-enable triggers
SET session_replication_role = default;

END
$BODY$
LANGUAGE plpgsql;