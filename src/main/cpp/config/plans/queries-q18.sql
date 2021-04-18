-- 0
SELECT l_orderkey, l_quantity
FROM lineitem
ORDER BY l_orderkey, l_quantity
-- 1
SELECT o_orderkey, o_custkey, o_totalprice, o_orderdate
FROM orders
ORDER BY o_orderkey, o_custkey, o_totalprice, o_orderdate
-- 2
SELECT l_orderkey, l_quantity
FROM lineitem
ORDER BY l_orderkey, l_quantity
-- 3
SELECT c_custkey, c_name
FROM customer
ORDER BY c_custkey
