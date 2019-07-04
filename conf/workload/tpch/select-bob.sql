-- work inside out with foreign key relationships

 -- may result in distributed joins with ps_suppkey 
DELETE FROM lineitem WHERE l_orderkey NOT IN (SELECT DISTINCT o_orderkey FROM orders, customer WHERE o_custkey = c_custkey AND c_nationkey > 12);
DELETE FROM orders  WHERE o_custkey  NOT IN (SELECT DISTINCT c_custkey FROM customer WHERE c_nationkey > 12);

-- need to delete foreign key reference constraint to allow for distributed joins
-- use unioned database for true constraints
ALTER TABLE lineitem DROP CONSTRAINT lineitem_l_partkey_fkey;
DELETE FROM partsupp WHERE ps_suppkey NOT IN (SELECT DISTINCT s_suppkey FROM supplier WHERE s_nationkey > 12);

DELETE FROM customer WHERE c_nationkey <= 12;
DELETE FROM supplier WHERE s_nationkey <= 12;


ANALYZE;