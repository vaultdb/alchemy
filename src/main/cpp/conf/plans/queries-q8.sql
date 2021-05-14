-- 0, collation: (1 ASC)
SELECT c_nationkey, c_custkey
FROM customer c JOIN nation n1 ON c_nationkey = n_nationkey
                JOIN region ON r_regionkey = n_regionkey
WHERE r_name = 'MIDDLE EAST'
ORDER BY c_custkey;
-- 1, collation: (0 ASC)
SELECT o_orderkey, o_custkey, o_orderyear AS o_year, NOT (o_orderdate >= DATE '1995-01-01' AND o_orderdate <= DATE '1996-12-31') AS dummy_tag
FROM orders
ORDER BY o_orderkey;
-- 3, collation: (1 ASC)
SELECT l_partkey, l_orderkey, l_suppkey, l_extendedprice * (1.0 - l_discount) volume
FROM part JOIN lineitem ON p_partkey = l_partkey
WHERE p_type = 'PROMO BRUSHED COPPER'
ORDER BY l_orderkey;
-- 4, collation: (0 ASC)
SELECT s_suppkey, CASE WHEN n_name = 'EGYPT' THEN 1.0 ELSE 0.0 END AS nation_check
FROM supplier JOIN nation ON n_nationkey = s_nationkey
ORDER BY s_suppkey;
