-- 0
SELECT c_custkey, c_mktsegment, NOT c_mktsegment = 'HOUSEHOLD ' AS dummy_tag
FROM customer
ORDER BY c_custkey
-- 1
SELECT o_orderkey, o_custkey, o_orderdate, o_shippriority, NOT o_orderdate < DATE '1995-03-25' AS dummy_tag
FROM orders
ORDER BY o_orderkey, o_custkey, o_orderdate, o_shippriority
-- 2
SELECT l_orderkey, l_extendedprice, l_discount, l_shipdate, NOT l_shipdate > DATE '1995-03-25' AS dummy_tag
FROM lineitem
ORDER BY l_orderkey, l_extendedprice, l_discount, l_shipdate
