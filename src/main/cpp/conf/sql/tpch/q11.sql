WITH percentile AS (
     SELECT SUM(psp.ps_supplycost * psp.ps_availqty) * 0.0001000000
     FROM
        partsupp psp,
        supplier sp,
        nation np
      where
        psp.ps_suppkey = sp.s_suppkey
        and sp.s_nationkey = np.n_nationkey
        and np.n_name = 'JAPAN'
    )
SELECT
  ps.ps_partkey,
  sum(ps.ps_supplycost * ps.ps_availqty) as val
FROM
  partsupp ps,
  supplier s,
  nation n
WHERE
  ps.ps_suppkey = s.s_suppkey
  and s.s_nationkey = n.n_nationkey
  and n.n_name = 'JAPAN'
GROUP BY ps.ps_partkey 
HAVING sum(ps.ps_supplycost * ps.ps_availqty) > (SELECT * FROM percentile)
ORDER BY val DESC