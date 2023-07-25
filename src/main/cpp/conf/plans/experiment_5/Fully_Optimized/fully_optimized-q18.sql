-- 0, collation: (0 ASC), party: 1
SELECT l_orderkey, sum(l_quantity) as sum_qty
FROM lineitem
GROUP BY l_orderkey
ORDER BY l_orderkey
-- 1, collation: (0 ASC), party: 2
SELECT l_orderkey, sum(l_quantity) as sum_qty
FROM lineitem
GROUP BY l_orderkey
ORDER BY l_orderkey
-- 6, collation: (0 ASC, 1 ASC)
SELECT o_orderkey, o_custkey, o_totalprice, o_orderdate
FROM orders
ORDER BY o_orderkey, o_custkey
-- 9, collation: (0 ASC)
SELECT L2.l_orderkey, L2.l_quantity
FROM lineitem L2
ORDER BY L2.l_orderkey
-- 12, collation: (0 ASC)
SELECT c_custkey, c_name
FROM customer
ORDER BY c_custkey