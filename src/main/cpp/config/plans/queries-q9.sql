-- 0
SELECT *
FROM (SELECT n_nationkey, n_name
FROM nation) AS t
INNER JOIN (SELECT s_suppkey, s_nationkey
FROM supplier) AS t0 ON t.n_nationkey = t0.s_nationkey
INNER JOIN (SELECT ps_partkey, ps_suppkey, ps_supplycost
FROM partsupp) AS t1 ON t0.s_suppkey = t1.ps_suppkey
INNER JOIN (SELECT p_partkey
FROM part
WHERE p_name LIKE '%yellow%') AS t3 ON t1.ps_partkey = t3.p_partkey
-- 1
SELECT l_orderkey, l_partkey, l_suppkey, l_quantity, l_extendedprice, l_discount
FROM lineitem
ORDER BY l_orderkey, l_partkey, l_suppkey, l_quantity, l_extendedprice, l_discount
-- 2
SELECT o_orderyear AS o_year, o_orderkey
FROM orders
ORDER BY o_orderkey
