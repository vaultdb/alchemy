DROP FUNCTION IF EXISTS truncate_tables;
CREATE FUNCTION truncate_tables(cutoff integer) RETURNS VOID AS
$BODY$
BEGIN

RAISE NOTICE 'customer';
EXECUTE 'CREATE TABLE temp AS (SELECT * FROM customer  ORDER BY c_custkey LIMIT ' || cutoff  || ')';
EXECUTE 'DROP TABLE IF EXISTS customer CASCADE';
EXECUTE 'ALTER TABLE temp RENAME TO customer';

RAISE NOTICE 'supplier';
EXECUTE 'CREATE TABLE temp AS (SELECT * FROM supplier ORDER BY s_suppkey LIMIT ' || cutoff / 15 || ')';
EXECUTE 'DROP TABLE IF EXISTS supplier CASCADE';
EXECUTE 'ALTER TABLE temp RENAME TO supplier';

RAISE NOTICE 'part';
EXECUTE 'CREATE TABLE temp AS (SELECT * FROM part ORDER BY p_partkey LIMIT ' || cutoff * 4 / 3 || ')';
EXECUTE 'DROP TABLE IF EXISTS part CASCADE';
EXECUTE 'ALTER TABLE temp RENAME TO part';

RAISE NOTICE 'orders';
EXECUTE 'CREATE TABLE temp AS (SELECT * FROM orders ORDER BY o_orderkey LIMIT ' || cutoff * 10 || ')';
EXECUTE 'DROP TABLE IF EXISTS orders CASCADE';
EXECUTE 'ALTER TABLE temp RENAME TO orders';

RAISE NOTICE 'lineitem';
EXECUTE 'CREATE TABLE temp AS (SELECT * FROM lineitem  ORDER BY l_orderkey, l_linenumber LIMIT ' || cutoff * 40 || ')';
EXECUTE 'DROP TABLE IF EXISTS lineitem CASCADE';
EXECUTE 'ALTER TABLE temp RENAME TO lineitem';

RAISE NOTICE 'partsupp';
EXECUTE 'CREATE TABLE temp AS (SELECT * FROM partsupp  ORDER BY ps_partkey, ps_suppkey LIMIT ' || cutoff * 16 / 3 || ')';
EXECUTE 'DROP TABLE IF EXISTS partsupp CASCADE';
EXECUTE 'ALTER TABLE temp RENAME TO partsupp';

END
$BODY$
LANGUAGE plpgsql;

