-- 0
SELECT o_orderkey, o_custkey, o_orderdate, o_shippriority, NOT o_orderdate < DATE '1995-03-25' AS dummy_tag
-- 1
SELECT c_custkey, NOT c_mktsegment = 'HOUSEHOLD' AS dummy_tag
FROM customer
-- 4
SELECT l_orderkey, l_extendedprice * (1 - l_discount) AS revenue, NOT l_shipdate > DATE '1995-03-25' AS dummy_tag
FROM lineitem
