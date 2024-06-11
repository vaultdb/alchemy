-- 0
SELECT l_orderkey, l_quantity
FROM lineitem
-- 4
SELECT L2.l_orderkey, L2.l_quantity
FROM lineitem L2
-- 7
SELECT o_orderkey, o_custkey, o_totalprice, o_orderdate
FROM orders
-- 10
SELECT c_custkey, c_name
FROM customer