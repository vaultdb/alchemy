-- 0, collation: (2 ASC, 3 ASC)
SELECT o_orderkey, o_custkey, o_orderdate, o_shippriority
FROM orders
WHERE o_orderdate < DATE '1995-03-25'
ORDER BY o_orderdate, o_shippriority
-- 1, collation: (0 ASC)
SELECT c_custkey
FROM customer
WHERE c_mktsegment = 'HOUSEHOLD'
ORDER BY c_custkey
-- 4, collation: (0 ASC)
SELECT l_orderkey, l_extendedprice * (1 - l_discount) AS revenue
FROM lineitem
WHERE l_shipdate > DATE '1995-03-25'
ORDER BY l_orderkey