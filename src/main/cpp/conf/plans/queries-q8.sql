-- 0
SELECT *
FROM (SELECT n_nationkey, n_regionkey
FROM nation) AS t
INNER JOIN (SELECT r_regionkey, r_name
FROM region
WHERE r_name = 'MIDDLE EAST              ') AS t1 ON t.n_regionkey = t1.r_regionkey
INNER JOIN (SELECT c_custkey, c_nationkey
FROM customer) AS t2 ON t.n_nationkey = t2.c_nationkey
-- 1
SELECT o_orderkey, o_custkey, o_orderyear AS o_year, NOT (o_orderdate >= DATE '1995-01-01' AND o_orderdate <= DATE '1996-12-31') AS dummy_tag
FROM orders
ORDER BY o_orderkey, o_custkey
-- 2
SELECT *
FROM (SELECT p_partkey, p_type
FROM part
WHERE p_type = 'PROMO BRUSHED COPPER') AS t0
INNER JOIN (SELECT l_orderkey, l_partkey, l_suppkey, l_extendedprice, l_discount
FROM lineitem) AS t1 ON t0.p_partkey = t1.l_partkey
-- 3
SELECT t0.s_suppkey, CASE WHEN t.n_name = 'EGYPT                    ' THEN 1 ELSE 0 END AS nation_check
FROM (SELECT n_nationkey, n_name
FROM nation) AS t
INNER JOIN (SELECT s_suppkey, s_nationkey
FROM supplier) AS t0 ON t.n_nationkey = t0.s_nationkey
