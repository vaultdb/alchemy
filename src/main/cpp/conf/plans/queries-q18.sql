-- 0, collation: (0 ASC, 1 ASC)
SELECT l_orderkey, l_quantity
FROM lineitem
ORDER BY l_orderkey, l_quantity;
-- 1, collation: (0 ASC, 1 ASC)
SELECT o_orderkey, o_custkey, o_totalprice, o_orderdate
FROM orders
ORDER BY o_orderkey, o_custkey;
-- 7, collation: (0 ASC, 1 ASC)
SELECT l_orderkey, l_quantity
FROM lineitem
ORDER BY l_orderkey, l_quantity;
-- 9, collation: (0 ASC)
SELECT c_custkey, c_name
FROM customer
ORDER BY c_custkey;
