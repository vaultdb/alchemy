-- 0, collation: (1 ASC, 2 ASC)
SELECT n_name,  ps_suppkey, ps_partkey, ps_supplycost
FROM nation JOIN supplier ON s_nationkey = n_nationkey
    JOIN partsupp ON s_suppkey = ps_suppkey
    JOIN part ON ps_partkey = p_partkey
WHERE p_name LIKE '%yellow%'
ORDER BY ps_suppkey, ps_partkey;
-- 1, collation: (1 ASC, 2 ASC, 3 ASC)
SELECT l_orderkey, l_partkey, l_suppkey, l_quantity::float4, l_extendedprice, l_discount
FROM lineitem
ORDER BY l_orderkey, l_partkey, l_suppkey;
-- 3, collation: (1 ASC)
SELECT o_orderyear AS o_year, o_orderkey
FROM orders
ORDER BY o_orderkey;
