-- 0
SELECT s_suppkey, n_name, CASE WHEN n_name = 'KENYA' THEN true ELSE false END AS nation_check
FROM supplier JOIN nation ON n_nationkey = s_nationkey
ORDER BY s_suppkey
-- 1
SELECT l_orderkey, l_partkey, l_suppkey, l_extendedprice, l_discount, CASE WHEN p_type = 'LARGE ANODIZED STEEL' AND p_partkey = l_partkey THEN FALSE
                                                                         ELSE TRUE
    END AS dummy_tag
FROM lineitem LEFT JOIN part ON p_partkey = l_partkey
ORDER BY l_orderkey
-- 3
SELECT o_orderkey, o_custkey, o_orderyear, o_orderdate < DATE '1995-03-25' AS dummy_tag
FROM orders
-- 5
SELECT c_custkey, c_nationkey
FROM customer JOIN nation ON c_nationkey = n_nationkey
   JOIN region ON n_regionkey = r_regionkey
WHERE r_name='AFRICA'
