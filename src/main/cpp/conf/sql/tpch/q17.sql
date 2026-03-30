SELECT COALESCE(sum(extendedprice), 0) / 7.0 as avg_yearly
FROM ( 
     SELECT l_quantity as quantity, l_extendedprice as extendedprice, t_avg_quantity
     FROM (
       SELECT l_partkey as t_partkey, 0.2 * avg(l_quantity) as t_avg_quantity 
       FROM lineitem
       GROUP By l_partkey) as tmp
    INNER JOIN (
      SELECT l_quantity, l_partkey, l_extendedprice
      FROM  part, lineitem 
      WHERE  p_partkey = l_partkey and p_brand = 'Brand#11' and p_container = 'MED BOX'
      ) as l1 ON  l1.l_partkey = t_partkey ) a
where quantity < t_avg_quantity