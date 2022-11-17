DROP FUNCTION IF EXISTS truncate_tables;
CREATE FUNCTION truncate_tables(cutoff integer) RETURNS VOID AS
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

