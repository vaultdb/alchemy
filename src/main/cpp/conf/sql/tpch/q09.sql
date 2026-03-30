WITH order_years AS (
     SELECT CAST(o_orderyear AS INT) as o_year, o.o_orderkey  
 FROM orders o),
     yellow_parts AS (
         SELECT p_partkey
      FROM part
      WHERE p_name like '%yellow%'),
     profit AS (
         SELECT   n_name, o_year, l.l_extendedprice * (1 - l.l_discount) - ps.ps_supplycost * l.l_quantity as amount
       FROM     yellow_parts p, supplier s, lineitem l, partsupp ps,  order_years o,  nation n 
    WHERE  s.s_suppkey = l.l_suppkey AND ps.ps_suppkey = l.l_suppkey AND ps.ps_partkey = l.l_partkey AND p.p_partkey = l.l_partkey AND o.o_orderkey = l.l_orderkey AND s.s_nationkey = n.n_nationkey)
 SELECT n_name, o_year, SUM(amount) as sum_profit
 FROM profit
 GROUP BY n_name, o_year 
 ORDER BY n_name, o_year DESC