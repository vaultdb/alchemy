-- 0
SELECT p_partkey, ps_supplycost, ps_suppkey, ps_partkey
FROM partsupp JOIN part ON ps_partkey = p_partkey
WHERE p_name LIKE '%yellow%'
-- 1
SELECT s_suppkey, s_nationkey
FROM supplier
-- 3
SELECT n_name AS nation, n_nationkey
FROM nation
-- 6
SELECT l_orderkey, l_partkey, l_suppkey, l_linenumber, l_quantity, l_extendedprice, l_discount, l_tax, l_returnflag, l_linestatus, l_shipdate, l_commitdate, l_receiptdate, l_shipinstruct, l_shipmode, l_comment
FROM lineitem
-- 9
SELECT o_orderkey, o_custkey, o_orderstatus, o_totalprice, o_orderdate, o_orderpriority, o_clerk, o_shippriority, o_comment, EXTRACT('year' FROM o_orderdate)::INT AS o_year
FROM orders