-- 0
SELECT c_custkey
FROM customer c JOIN nation n1 ON c_nationkey = n_nationkey
                JOIN region ON r_regionkey = n_regionkey
WHERE r_name = 'AFRICA'
ORDER BY c_custkey
-- 1
SELECT o_orderkey, o_custkey, o_orderstatus, o_totalprice, o_orderdate, o_orderpriority, o_clerk, o_shippriority, o_comment, EXTRACT('year' FROM o_orderdate)::INT AS o_year, NOT (o_orderdate >= date '1995-01-01' and o_orderdate < date '1996-12-31') dummy_tag
FROM orders
ORDER BY o_custkey, o_orderkey;
-- 4
SELECT l_orderkey, l_partkey, l_suppkey, l_linenumber, l_quantity, l_extendedprice, l_discount, l_tax, l_returnflag, l_linestatus, l_shipdate, l_commitdate, l_receiptdate, l_shipinstruct, l_shipmode, l_comment
FROM lineitem
ORDER BY l_orderkey;
-- 7
SELECT p_partkey, p_name, p_mfgr, p_brand, p_type, p_size, p_container, p_retailprice, p_comment
FROM part
WHERE p_type = 'LARGE ANODIZED STEEL'
-- 10
SELECT s_suppkey, s_name, s_address, s_nationkey, s_phone, s_acctbal, s_comment
FROM supplier
ORDER BY s_suppkey
-- 13
SELECT n2.n_nationkey, n2.n_name, n2.n_regionkey, n2.n_comment, CASE WHEN n2.n_name = 'KENYA' THEN 1.0 ELSE 0.0 END AS nation_check
FROM nation n2
