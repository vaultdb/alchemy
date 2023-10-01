CREATE FUNCTION split_tables_alice() RETURNS VOID AS
    $BODY$
DECLARE
median_key INTEGER;
BEGIN
    -- Disable triggers
    SET session_replication_role = replica;

    RAISE NOTICE 'customer';
    SELECT c_custkey INTO median_key FROM (
                                              SELECT c_custkey FROM customer ORDER BY c_custkey LIMIT 2 - MOD((SELECT COUNT(*) FROM customer), 2) OFFSET (SELECT (COUNT(*) - 1) / 2 FROM customer)
                                          ) AS subquery ORDER BY c_custkey DESC LIMIT 1;
    EXECUTE 'DELETE FROM customer WHERE c_custkey > ' || median_key;

    RAISE NOTICE 'supplier';
    SELECT s_suppkey INTO median_key FROM (
                                               SELECT s_suppkey FROM supplier ORDER BY s_suppkey LIMIT 2 - MOD((SELECT COUNT(*) FROM supplier), 2) OFFSET (SELECT (COUNT(*) - 1) / 2 FROM supplier)
                                           ) AS subquery ORDER BY s_suppkey DESC LIMIT 1;
    EXECUTE 'DELETE FROM supplier WHERE s_suppkey > ' || median_key;

    RAISE NOTICE 'orders';
    SELECT o_orderkey INTO median_key FROM (
                                              SELECT o_orderkey FROM orders ORDER BY o_orderkey LIMIT 2 - MOD((SELECT COUNT(*) FROM orders), 2) OFFSET (SELECT (COUNT(*) - 1) / 2 FROM orders)
                                          ) AS subquery ORDER BY o_orderkey DESC LIMIT 1;
    EXECUTE 'DELETE FROM orders WHERE o_orderkey > ' || median_key;

    RAISE NOTICE 'lineitem';
    SELECT l_orderkey INTO median_key FROM (
                                               SELECT l_orderkey FROM lineitem ORDER BY l_orderkey LIMIT 2 - MOD((SELECT COUNT(*) FROM lineitem), 2) OFFSET (SELECT (COUNT(*) - 1) / 2 FROM lineitem)
                                           ) AS subquery ORDER BY l_orderkey DESC LIMIT 1;
    EXECUTE 'DELETE FROM lineitem WHERE l_orderkey > ' || median_key;

    RAISE NOTICE 'partsupp';
    SELECT ps_partkey INTO median_key FROM (
                                               SELECT ps_partkey FROM partsupp ORDER BY ps_partkey LIMIT 2 - MOD((SELECT COUNT(*) FROM partsupp), 2) OFFSET (SELECT (COUNT(*) - 1) / 2 FROM partsupp)
                                           ) AS subquery ORDER BY ps_partkey DESC LIMIT 1;
    EXECUTE 'DELETE FROM partsupp WHERE ps_partkey > ' || median_key;

    -- Re-enable triggers
    SET session_replication_role = default;
END
$BODY$
LANGUAGE plpgsql;

