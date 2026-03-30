WITH complaints AS (SELECT * FROM supplier WHERE s_comment LIKE '%Customer%Complaints%'),
     ps_suppkey_set_diff AS (
     SELECT ps_partkey, ps_suppkey
     FROM partsupp ps LEFT JOIN complaints c ON ps.ps_suppkey = c.s_suppkey
     WHERE c.s_suppkey IS NULL),

     part_pro  AS ( SELECT p_partkey, p_brand,p_type,p_size
   FROM part p 
      WHERE p.p_brand <> 'Brand#21'
      AND p.p_type not like 'MEDIUM PLATED%'
        AND p.p_size IN (38, 2, 8, 31, 44, 5, 14, 24)
      )

SELECT
   p.p_brand,
   p.p_type,
   p.p_size,
   COUNT(DISTINCT ps.ps_suppkey)::INT as supplier_cnt
 FROM
   ps_suppkey_set_diff ps,
   part_pro p
 WHERE
   p.p_partkey = ps.ps_partkey
   
 GROUP BY
   p.p_brand,
   p.p_type,
   p.p_size
 ORDER BY
   supplier_cnt desc,
   p.p_brand,
   p.p_type,
   p.p_size