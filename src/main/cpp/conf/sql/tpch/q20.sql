 WITH antiques AS (SELECT p.p_partkey 
                                  from 
                                    part p 
                                  where 
                                    p.p_name like 'antique%' 
                                 ), 
             lineitem_agg AS ( 
                             select 
                                    l_partkey, l_suppkey, 0.5 * sum(l.l_quantity) inventory 
                                  from 
                                    lineitem l 
                                  where 
                                     l.l_shipdate >= date '1993-01-01' 
                                    and l.l_shipdate < date '1994-01-01' 
                               GROUP BY l_partkey, l_suppkey 
                      ), 
  
             ps_qualified AS ( 
 	        SELECT ps_suppkey, ps_partkey, ps.ps_availqty 
                 FROM  partsupp ps LEFT JOIN antiques a ON ps.ps_partkey = a.p_partkey 
                  WHERE  a.p_partkey IS NOT NULL), 
             qualified AS ( 
                           SELECT DISTINCT ps.ps_suppkey 
                           FROM lineitem_agg la INNER JOIN ps_qualified ps ON la.l_partkey = ps.ps_partkey AND la.l_suppkey = ps.ps_suppkey 
                            WHERE ps.ps_availqty > la.inventory 
                             ) 
 select 
                         s.s_name, 
                         s.s_address 
                       from 
                         supplier s LEFT JOIN qualified q ON s.s_suppkey = q.ps_suppkey, 
                         nation n 
                       where 
                         ps_suppkey IS NOT NULL 
                         and s.s_nationkey = n.n_nationkey 
                         and n.n_name = 'KENYA'
                       order by 
                         s.s_name 