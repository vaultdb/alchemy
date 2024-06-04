-- 0, collation: (0 ASC)
SELECT c_custkey, c_nationkey
FROM customer JOIN nation ON c_nationkey = n_nationkey
JOIN region ON n_regionkey = r_regionkey
WHERE r_name = 'EUROPE'
ORDER BY c_custkey, c_nationkey
-- 1, collation: (1 ASC)
SELECT o_orderkey, o_custkey, NOT (o_orderdate >= DATE '1993-01-01' AND o_orderdate < DATE '1994-01-01') AS dummy_tag
FROM orders
ORDER BY o_custkey, o_orderkey;
-- 4, collation: (0 ASC)
SELECT l_orderkey, l_suppkey, l_extendedprice * (1 - l_discount) AS revenue
FROM lineitem
ORDER BY l_orderkey, revenue DESC
-- 7
SELECT s_suppkey, s_nationkey, n_name
FROM supplier JOIN nation ON s_nationkey = n_nationkey
   JOIN region ON n_regionkey = r_regionkey
WHERE r_name = 'EUROPE'
ORDER BY s_nationkey, s_suppkey
