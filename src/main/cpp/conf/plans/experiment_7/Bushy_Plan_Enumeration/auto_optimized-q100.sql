-- 0
SELECT r_regionkey
FROM region
-- 1
SELECT N1.n_nationkey, N1.n_regionkey
FROM nation N1
-- 4
SELECT c_custkey, c_nationkey
FROM customer
-- 6
SELECT o_orderkey, o_custkey
FROM orders
-- 9
SELECT l_orderkey, l_partkey, l_suppkey
FROM lineitem
-- 12
SELECT ps_partkey, ps_suppkey
FROM partsupp
-- 15
SELECT p_partkey
FROM part
-- 18
SELECT s_suppkey, s_nationkey
FROM supplier
-- 21
SELECT N2.n_nationkey, N2.n_regionkey
FROM nation N2