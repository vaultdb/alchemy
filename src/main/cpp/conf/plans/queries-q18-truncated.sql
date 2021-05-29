-- 0, collation: (0 ASC, 1 ASC)
SELECT l_orderkey, l_quantity
FROM lineitem
WHERE l_orderkey IN (1263015, 1544643, 2199712, 2869152, 3043270, 3767271, 4702759, 4722021, 4806726)
ORDER BY l_orderkey, l_quantity;
-- 1, collation: (0 ASC, 1 ASC)
SELECT o_orderkey, o_custkey, o_totalprice, o_orderdate
FROM orders
WHERE o_orderkey IN (1263015, 1544643, 2199712, 2869152, 3043270, 3767271, 4702759, 4722021, 4806726)
ORDER BY o_orderkey, o_custkey;
-- 8, collation: (0 ASC)
SELECT c_custkey, c_name
FROM customer
WHERE c_custkey IN (15619, 36619, 66790, 117919, 119989, 126865, 128120, 144617, 147197)
ORDER BY c_custkey;
