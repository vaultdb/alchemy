-- 0
SELECT t2.s_suppkey, t2.s_nationkey, t1.n_name, t1.n_nationkey
FROM (SELECT r_regionkey, r_name
      FROM region
      WHERE r_name = 'EUROPE') AS t0
         INNER JOIN (SELECT n_nationkey, n_regionkey, n_name
                     FROM nation) AS t1 ON t0.r_regionkey = t1.n_regionkey
         INNER JOIN (SELECT s_suppkey, s_nationkey
                     FROM supplier) AS t2 ON t1.n_nationkey = t2.s_nationkey
-- 1
SELECT c_custkey, c_name, c_address, c_nationkey, c_phone, c_acctbal, c_mktsegment, c_comment
FROM customer
-- 4
SELECT o_orderkey, o_custkey, o_orderstatus, o_totalprice, o_orderdate, o_orderpriority, o_clerk, o_shippriority, o_comment, NOT (o_orderdate >= date '1993-01-01' and o_orderdate < date '1994-01-01') dummy_tag
FROM orders
-- 7
SELECT l_orderkey, l_partkey, l_suppkey, l_linenumber, l_quantity, l_extendedprice, l_discount, l_tax, l_returnflag, l_linestatus, l_shipdate, l_commitdate, l_receiptdate, l_shipinstruct, l_shipmode, l_comment
FROM lineitem