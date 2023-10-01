-- 0
SELECT c_custkey, c_mktsegment = 'HOUSEHOLD ' AS dummy_tag
FROM (SELECT c_custkey, c_mktsegment
FROM customer) AS t
-- 1
SELECT *, o_orderdate < DATE '1995-03-25' AS dummy_tag
FROM (SELECT o_orderkey, o_custkey, o_orderdate, o_shippriority
FROM orders) AS t
-- 3
SELECT l_orderkey, l_extendedprice, l_discount, l_shipdate > DATE '1995-03-25' AS dummy_tag
FROM (SELECT l_orderkey, l_extendedprice, l_discount, l_shipdate
FROM lineitem) AS t
