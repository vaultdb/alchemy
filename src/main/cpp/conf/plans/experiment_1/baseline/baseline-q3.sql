-- 0
SELECT c_custkey, NOT c_mktsegment = 'HOUSEHOLD ' AS dummy_tag
FROM customer
ORDER BY c_custkey
-- 1
SELECT o_orderkey, o_custkey, o_orderstatus, o_totalprice, o_orderdate, o_orderpriority, o_clerk, o_shippriority, o_comment, NOT o_orderdate < DATE '1995-03-25' AS dummy_tag
FROM orders
ORDER BY o_orderkey, o_orderdate, o_shippriority
-- 4
SELECT l_orderkey, l_partkey, l_suppkey, l_linenumber, l_quantity, l_extendedprice, l_discount, l_tax, l_returnflag, l_linestatus, l_shipdate, l_commitdate, l_receiptdate, l_shipinstruct, l_shipmode, l_comment, NOT l_shipdate > DATE '1995-03-25' AS dummy_tag
FROM lineitem
ORDER BY l_orderkey
