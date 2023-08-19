-- 0, collation: (1 ASC, 0 ASC)
SELECT s_suppkey, n_name
FROM supplier JOIN nation ON s_nationkey = n_nationkey
ORDER BY n_name, s_suppkey
-- 1, collation: (1 ASC, 0 ASC)
SELECT ps_partkey, ps_suppkey, ps_supplycost
FROM partsupp JOIN part ON ps_partkey = p_partkey
WHERE p_name LIKE '%yellow%'
ORDER BY ps_suppkey, ps_partkey
-- 5, collation: (2 ASC, 1 ASC)
SELECT l_orderkey, l_partkey, l_suppkey, l_quantity::float4, l_extendedprice, l_discount
FROM lineitem
ORDER BY l_suppkey, l_partkey
-- 9, collation: (1 ASC)
SELECT EXTRACT('year' FROM o_orderdate)::INT AS o_year, o_orderkey
FROM orders
ORDER BY o_orderkey
