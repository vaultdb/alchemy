-- 0
SELECT l_orderkey, l_quantity
FROM lineitem
-- 4
SELECT o_orderkey, o_custkey, o_totalprice, o_orderdate
FROM orders
-- 5
SELECT c_custkey, c_name
FROM customer
-- 10
SELECT L2.l_orderkey, L2.l_quantity
FROM lineitem L2