-- 0
SELECT l_orderkey, l_partkey, l_suppkey, l_linenumber, l_quantity, l_extendedprice, l_discount, l_tax, l_returnflag, l_linestatus, l_shipdate, l_commitdate, l_receiptdate, l_shipinstruct, l_shipmode, l_comment, l_extendedprice * (1 - l_discount) AS discount, l_extendedprice * (1 - l_discount) * (1 + l_tax) AS charge, NOT l_shipdate <= DATE '1998-08-03' AS dummy_tag
FROM lineitem
ORDER BY l_returnflag, l_linestatus, l_orderkey, l_linenumber;