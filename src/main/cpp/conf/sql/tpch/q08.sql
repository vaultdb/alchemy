SELECT   o_year, SUM(CASE WHEN  n_nation = 'KENYA' THEN volume ELSE 0.0 END) / SUM(VOLUME) AS mkt_share
FROM ( SELECT CAST(o_orderyear AS INT) AS o_year, l.l_extendedprice * (1 - l.l_discount) AS volume,  n2.n_name as n_nation
       FROM part p, supplier s, lineitem l, orders o, customer c, nation n1, nation n2, region r
       WHERE p.p_partkey = l.l_partkey AND s.s_suppkey = l.l_suppkey AND l.l_orderkey = o.o_orderkey AND  o.o_custkey = c.c_custkey
             AND c.c_nationkey = n1.n_nationkey AND n1.n_regionkey = r.r_regionkey AND r.r_name = 'AFRICA' AND s.s_nationkey = n2.n_nationkey
             AND o.o_orderdate BETWEEN DATE '1995-01-01' AND DATE '1996-12-31' AND  p.p_type = 'LARGE ANODIZED STEEL') AS all_nations
GROUP BY o_year
ORDER BY o_year