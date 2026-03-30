SELECT o_orderkey,  sum(l.l_extendedprice * (1 - l.l_discount)) as revenue, o_orderdate, o_shippriority
 FROM customer c JOIN orders o ON  c.c_custkey = o.o_custkey
     JOIN lineitem l ON l.l_orderkey = o.o_orderkey
 WHERE  c.c_mktsegment = 'HOUSEHOLD'  AND o.o_orderdate < date '1995-03-25' AND l.l_shipdate > date '1995-03-25'
 GROUP BY  o_orderkey, o.o_orderdate,  o.o_shippriority
 ORDER BY  revenue DESC, o.o_orderdate
 LIMIT 10