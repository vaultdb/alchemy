-- 0, collation: (0 ASC)
SELECT c_custkey
FROM customer c JOIN nation n1 ON c_nationkey = n_nationkey
                JOIN region ON r_regionkey = n_regionkey
WHERE r_name = 'AFRICA'
ORDER BY c_custkey
-- 1, collation: (1 ASC, 0 ASC)
SELECT o_orderkey, o_custkey, o_orderyear::INT AS o_year, NOT (o_orderdate >= date '1995-01-01' and o_orderdate < date '1996-12-31') dummy_tag
FROM orders
ORDER BY o_custkey, o_orderkey
-- 4, collation: (0 ASC)
SELECT l_orderkey, l_suppkey, l_partkey, l_extendedprice * (1.0 - l_discount) AS volume
FROM lineitem
ORDER BY l_orderkey
-- 7, collation: (0 ASC)
SELECT s_suppkey, CASE WHEN n_name = 'KENYA' THEN true ELSE false END AS nation_check
FROM supplier JOIN nation ON n_nationkey = s_nationkey
ORDER BY s_suppkey
-- 10, collation: (0 ASC)
SELECT p_partkey
FROM part
WHERE p_type = 'LARGE ANODIZED STEEL'
ORDER BY p_partkey
