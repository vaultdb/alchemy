-- 0
SELECT s_suppkey, n_name
FROM supplier JOIN nation ON n_nationkey = s_nationkey
ORDER BY s_suppkey
-- 1
SELECT l_orderkey, l_partkey, l_suppkey, l_quantity, l_extendedprice, l_discount,
       CASE WHEN p_name like '%yellow%' AND p_partkey = l_partkey THEN FALSE ELSE TRUE END AS dummy_tag
FROM lineitem LEFT JOIN part ON p_partkey = l_partkey
ORDER BY l_orderkey
-- 3
SELECT ps_partkey, ps_suppkey, ps_supplycost
FROM partsupp
-- 5
SELECT o_orderkey, o_orderyear
FROM orders
