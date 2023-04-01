DROP FUNCTION IF EXISTS truncate_tables;
CREATE FUNCTION truncate_tables(cutoff integer) RETURNS VOID AS
$BODY$
BEGIN
RAISE NOTICE 'Deleting rows from customer, orders, and lineitem';
EXECUTE 'DELETE FROM customer WHERE c_custkey > ' || cutoff;

RAISE NOTICE 'Deleting rows from partsupp';

EXECUTE 'DELETE FROM partsupp
    WHERE NOT EXISTS (SELECT  * FROM lineitem l  WHERE l_partkey = ps_partkey AND l_suppkey = ps_suppkey);';

RAISE NOTICE 'Deleting rows from supplier';

EXECUTE 'DELETE FROM supplier
WHERE NOT EXISTS (SELECT * FROM partsupp WHERE ps_suppkey = s_suppkey)';


RAISE NOTICE 'Deleting rows from part';

EXECUTE 'DELETE FROM part
WHERE NOT EXISTS (SELECT * FROM partsupp WHERE ps_partkey = p_partkey);';


END
$BODY$
LANGUAGE plpgsql;

