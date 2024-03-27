-- 0
SELECT r_regionkey
FROM region
-- 1
SELECT n_nationkey, n_regionkey
FROM nation
-- 4
SELECT c_custkey, c_nationkey
FROM customer
-- 7
SELECT o_orderkey, o_custkey
FROM orders
-- 10
SELECT l_orderkey, l_partkey, l_suppkey
FROM lineitem
-- 13
SELECT ps_partkey, ps_suppkey
FROM partsupp