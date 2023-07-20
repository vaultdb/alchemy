-- 0, collation: (1 ASC, 0 ASC)
SELECT l_orderkey, l_quantity
FROM lineitem
ORDER BY l_quantity, l_orderkey
-- 4, collation: (0 ASC)
SELECT L2.l_orderkey, L2.l_quantity
FROM lineitem L2
ORDER BY L2.l_orderkey
-- 7, collation: (0 ASC, 1 ASC)
SELECT o_orderkey, o_custkey, o_totalprice, o_orderdate
FROM orders
ORDER BY o_orderkey, o_custkey
-- 10, collation: (0 ASC)
SELECT c_custkey, c_name
FROM customer
ORDER BY c_custkey