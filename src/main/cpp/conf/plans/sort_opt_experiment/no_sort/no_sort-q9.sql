-- 0
SELECT s_suppkey, n_name
FROM supplier JOIN nation ON s_nationkey = n_nationkey
-- 1
SELECT ps_partkey, ps_suppkey, ps_supplycost
FROM partsupp JOIN part ON ps_partkey = p_partkey
WHERE p_name LIKE '%yellow%'
-- 4
SELECT l_orderkey, l_partkey, l_suppkey, l_quantity::float4, l_extendedprice, l_discount
FROM lineitem
-- 7
SELECT EXTRACT('year' FROM o_orderdate)::INT AS o_year, o_orderkey
FROM orders
