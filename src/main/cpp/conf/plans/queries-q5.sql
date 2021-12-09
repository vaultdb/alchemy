-- 0, collation: (1 ASC)
SELECT t1.n_name, t2.c_custkey, t2.c_nationkey
FROM (SELECT r_regionkey, r_name
FROM region
WHERE r_name = 'EUROPE') AS t0
INNER JOIN (SELECT n_nationkey, n_name, n_regionkey
FROM nation) AS t1 ON t0.r_regionkey = t1.n_regionkey
INNER JOIN (SELECT c_custkey, c_nationkey
FROM customer) AS t2 ON t1.n_nationkey = t2.c_nationkey
WHERE c_custkey = 136 OR c_custkey = 31
   OR c_custkey = 137 OR c_custkey = 32
ORDER BY t2.c_custkey ASC;
-- 1, collation: (0 ASC, 1 ASC, 2 ASC)
SELECT o_orderkey, o_custkey, o_orderdate
FROM orders
WHERE o_orderkey = 4204389 OR o_orderkey = 3387013 OR o_orderkey = 4692995
ORDER BY o_orderkey, o_custkey, o_orderdate;
-- 3, collation: (0 ASC, 1 ASC, 2 ASC, 3 ASC)
SELECT l_orderkey, l_suppkey, l_extendedprice, l_discount
FROM lineitem
WHERE l_orderkey = 4204389 OR l_orderkey = 3387013 OR l_orderkey = 4692995
   OR l_orderkey = 4204390 OR l_orderkey = 3387014 OR l_orderkey = 4692996
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
WHERE s_suppkey = 53 OR s_suppkey = 182 OR s_suppkey = 147
   OR s_suppkey = 54 OR s_suppkey = 183 OR s_suppkey = 148
ORDER BY s_suppkey