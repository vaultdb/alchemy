-- 0
SELECT c_custkey
FROM customer c JOIN nation n1 ON c_nationkey = n_nationkey
                JOIN region ON r_regionkey = n_regionkey
WHERE r_name = 'AFRICA'
-- 1
SELECT o_orderkey, o_custkey, o_orderyear::INT AS o_year, NOT (o_orderdate >= date '1995-01-01' and o_orderdate < date '1996-12-31') dummy_tag
FROM orders
-- 4
SELECT l_orderkey, l_suppkey, l_extendedprice * (1.0 - l_discount) AS volume,
       CASE
           WHEN p_type = 'LARGE ANODIZED STEEL' AND p_partkey = l_partkey THEN FALSE
           ELSE TRUE
           END AS dummy_tag
FROM lineitem LEFT JOIN part ON p_partkey = l_partkey
-- 7
SELECT s_suppkey, CASE WHEN n_name = 'KENYA' THEN true ELSE false END AS nation_check
FROM supplier JOIN nation ON n_nationkey = s_nationkey
