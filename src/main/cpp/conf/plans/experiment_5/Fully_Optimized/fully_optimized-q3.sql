-- 0, collation: (0 ASC)
SELECT o_orderkey, o_custkey, o_orderdate, o_shippriority, NOT o_orderdate < DATE '1995-03-25' AS dummy_tag
FROM orders
ORDER BY o_orderkey, o_orderdate, o_shippriority
-- 1
SELECT c_custkey, NOT c_mktsegment = 'HOUSEHOLD' AS dummy_tag
FROM customer
ORDER BY c_custkey
-- 4, collation: (0 ASC)
SELECT l_orderkey, l_extendedprice * (1 - l_discount) AS revenue, NOT l_shipdate > DATE '1995-03-25' AS dummy_tag
FROM lineitem
ORDER BY l_orderkey
