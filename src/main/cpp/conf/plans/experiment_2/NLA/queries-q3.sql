-- 0, collation: (0 ASC, 1 ASC, 2 ASC)
SELECT l_orderkey, o_orderdate, o_shippriority, l.l_extendedprice * (1 - l.l_discount) as revenue, NOT (c.c_mktsegment = 'HOUSEHOLD'  AND o.o_orderdate < date '1995-03-25' AND l.l_shipdate > date '1995-03-25') AS dummy_tag
 FROM customer c JOIN orders o ON  c.c_custkey = o.o_custkey
                     JOIN lineitem l ON l.l_orderkey = o.o_orderkey
                 WHERE c_custkey < 100
ORDER BY l_orderkey, o_orderdate, o_shippriority
