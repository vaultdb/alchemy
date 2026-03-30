SELECT n.n_name, SUM(l.l_extendedprice * (1 - l.l_discount)) as revenue
 FROM  customer c JOIN orders o ON c.c_custkey = o.o_custkey
     JOIN lineitem l ON l.l_orderkey = o.o_orderkey
     JOIN supplier s ON l.l_suppkey = s.s_suppkey
     JOIN nation n ON s.s_nationkey = n.n_nationkey
     JOIN region r ON n.n_regionkey = r.r_regionkey
WHERE c.c_nationkey = s.s_nationkey  AND r.r_name = 'EUROPE' AND o.o_orderdate >= date '1993-01-01' AND o.o_orderdate < date '1994-01-01'
 GROUP BY   n.n_name
 ORDER BY revenue DESC