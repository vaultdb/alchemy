-- 0, collation: (0 ASC, 1 ASC)
SELECT l_orderkey, l_quantity
FROM lineitem
WHERE l_orderkey IN (6882, 29158, 502886, 551136, 565574)
ORDER BY l_orderkey, l_quantity;
-- 1, collation: (0 ASC, 1 ASC)
SELECT o_orderkey, o_custkey, o_totalprice, o_orderdate
FROM orders
WHERE o_orderkey IN (6882, 29158, 502886, 551136, 565574)
ORDER BY o_orderkey, o_custkey;
-- 7, collation: (0 ASC, 1 ASC)
SELECT l_orderkey, l_quantity
FROM lineitem
WHERE l_orderkey IN (6882, 29158, 502886, 551136, 565574)
ORDER BY l_orderkey, l_quantity;
-- 9, collation: (0 ASC)
SELECT c_custkey, c_name
FROM customer
WHERE c_custkey IN (1639, 1775, 6655, 11459, 14110)
ORDER BY c_custkey;
