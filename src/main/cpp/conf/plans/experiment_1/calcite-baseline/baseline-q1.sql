-- 0
SELECT l_returnflag, l_linestatus, l_quantity, l_extendedprice, l_extendedprice * (1 - l_discount) AS disc_price, l_extendedprice * (1 - l_discount) * (1 + l_tax) AS amt, l_discount, l_shipdate <= DATE '1998-08-03' AS dummy_tag
FROM (SELECT l_quantity, l_extendedprice, l_discount, l_tax, l_returnflag, l_linestatus, l_shipdate
FROM lineitem) AS t
