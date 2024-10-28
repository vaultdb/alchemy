-- 0, collation: (0 ASC, 1 ASC, 2 ASC, 3 ASC)
SELECT l_returnflag, l_linestatus, l_quantity, l_extendedprice, l_extendedprice * (1 - l_discount) AS discount, l_extendedprice * (1 - l_discount) * (1 + l_tax) AS charge, l_discount
FROM lineitem
WHERE l_shipdate <= DATE '1998-08-03'
ORDER BY  l_returnflag, l_linestatus, l_orderkey, l_linenumber;