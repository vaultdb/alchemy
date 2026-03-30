WITH c_orders AS (
     SELECT c.c_custkey, COUNT(o.o_orderkey) c_count   
     FROM   customer c  LEFT OUTER JOIN orders o ON c.c_custkey = o.o_custkey  AND o.o_comment NOT LIKE '%special%requests%'   
     GROUP BY   c.c_custkey )
SELECT  c_count::INT,   count(*) as custdist
FROM    c_orders
GROUP BY c_count 
ORDER BY custdist DESC, c_count DESC