-- 0, collation: (0 ASC, 1 ASC)
SELECT l_returnflag, l_linestatus, l_quantity, l_extendedprice, l_extendedprice * (1 - l_discount) AS discount, l_extendedprice * (1 - l_discount) * (1 + l_tax) AS charge, l_discount, NOT l_shipdate <= DATE '1998-08-03' AS dummy_tag
FROM lineitem
ORDER BY  l_returnflag, l_linestatus, l_orderkey, l_linenumber;