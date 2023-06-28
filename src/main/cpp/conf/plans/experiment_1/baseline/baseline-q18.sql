-- 0
SELECT l_orderkey, l_partkey, l_suppkey, l_linenumber, l_quantity, l_extendedprice, l_discount, l_tax, l_returnflag, l_linestatus, l_shipdate, l_commitdate, l_receiptdate, l_shipinstruct, l_shipmode, l_comment
FROM lineitem
-- 4
SELECT L2.l_orderkey, L2.l_partkey, L2.l_suppkey, L2.l_linenumber, L2.l_quantity, L2.l_extendedprice, L2.l_discount, L2.l_tax, L2.l_returnflag, L2.l_linestatus, L2.l_shipdate, L2.l_commitdate, L2.l_receiptdate, L2.l_shipinstruct, L2.l_shipmode, L2.l_comment
FROM lineitem L2
-- 7
SELECT o_orderkey, o_orderdate, o_totalprice, o_custkey
FROM orders
-- 10
SELECT c_name, c_custkey
FROM customer
