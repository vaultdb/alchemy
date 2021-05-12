-- 0, collation: (1 ASC)
SELECT t1.n_name, t2.c_custkey, t2.c_nationkey
FROM (SELECT r_regionkey, r_name
FROM region
WHERE r_name = 'EUROPE') AS t0
INNER JOIN (SELECT n_nationkey, n_name, n_regionkey
FROM nation) AS t1 ON t0.r_regionkey = t1.n_regionkey
INNER JOIN (SELECT c_custkey, c_nationkey
FROM customer) AS t2 ON t1.n_nationkey = t2.c_nationkey
ORDER BY t2.c_custkey ASC
-- 1, collation: (0 ASC, 1 ASC, 2 ASC)
SELECT o_orderkey, o_custkey, o_orderdate, NOT (o_orderdate >= DATE '1997-01-01' AND o_orderdate < DATE '1998-01-01') AS dummy_tag
FROM orders
ORDER BY o_orderkey, o_custkey, o_orderdate;
-- 3, collation: (0 ASC, 1 ASC, 2 ASC, 3 ASC)
SELECT l_orderkey, l_suppkey, l_extendedprice, l_discount
FROM lineitem
ORDER BY l_orderkey, l_suppkey, l_extendedprice, l_discount
-- 5, collation: (0 ASC)
SELECT t2.s_suppkey, t2.s_nationkey
FROM (SELECT r_regionkey, r_name
FROM region
WHERE r_name = 'EUROPE') AS t0
INNER JOIN (SELECT n_nationkey, n_regionkey
FROM nation) AS t1 ON t0.r_regionkey = t1.n_regionkey
INNER JOIN (SELECT s_suppkey, s_nationkey
FROM supplier) AS t2 ON t1.n_nationkey = t2.s_nationkey
ORDER BY s_suppkey