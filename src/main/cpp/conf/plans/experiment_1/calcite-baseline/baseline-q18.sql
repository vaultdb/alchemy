-- 0
SELECT c_custkey, c_name
FROM customer
-- 1
SELECT o_orderkey, o_custkey, o_totalprice, o_orderdate
FROM orders
-- 3
SELECT l_orderkey, l_quantity
FROM lineitem
-- 5
SELECT l_orderkey, SUM(l_quantity) > 313 AS dummy_tag
FROM lineitem
GROUP BY l_orderkey
