-- 0
SELECT c_custkey, c_nationkey
FROM customer
-- 1
SELECT o_orderkey, o_custkey, o_orderdate >= DATE '1993-01-01' AND o_orderdate < DATE '1994-01-01' AS dummy_tag
FROM (SELECT o_orderkey, o_custkey, o_orderdate
FROM orders) AS t
-- 3
SELECT l_orderkey, l_suppkey, l_extendedprice, l_discount
FROM lineitem
-- 5
SELECT s_suppkey, s_nationkey, n_name
FROM supplier JOIN nation ON s_nationkey = n_nationkey
     JOIN region ON n_regionkey = r_regionkey
WHERE r_name='EUROPE'
