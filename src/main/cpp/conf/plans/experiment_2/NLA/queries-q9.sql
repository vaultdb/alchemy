-- 0
WITH order_years AS (
                     SELECT extract(year from o_orderdate)::INT as o_year, o.o_orderkey
                     FROM orders o),
                     yellow_parts AS (
                         SELECT p_partkey
                	 FROM part
                	 WHERE p_name like '%yellow%'),
                     profit AS (
                         select
                    	   n_name,
                     	    o_year,
                      	    l.l_extendedprice * (1 - l.l_discount) - ps.ps_supplycost * l.l_quantity as amount
                    	  from
                    	    yellow_parts p,
                      	    supplier s,
                      	    lineitem l,
                      	    partsupp ps,
                      	    order_years o,
                      	    nation n
                    where
                      s.s_suppkey = l.l_suppkey
                      and ps.ps_suppkey = l.l_suppkey
                      and ps.ps_partkey = l.l_partkey
                      and p.p_partkey = l.l_partkey
                      and o.o_orderkey = l.l_orderkey
                      and s.s_nationkey = n.n_nationkey
                      and l_orderkey IN (SELECT o_orderkey FROM orders where o_custkey < 100))
                 select
                  n_name,
                  o_year,
                  amount
                 from profit