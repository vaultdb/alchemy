-- 0
SELECT l_returnflag, l_linestatus, l_quantity, l_extendedprice, l_extendedprice * (1 - l_discount) AS discount, l_extendedprice * (1 - l_discount) * (1 + l_tax) AS charge, l_discount, NOT l_shipdate <= DATE '1998-08-03' AS dummy_tag
FROM lineitem
