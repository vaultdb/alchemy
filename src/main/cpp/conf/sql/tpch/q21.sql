WITH partial_agg AS (SELECT l_orderkey, MIN(CASE WHEN l_receiptdate > l_commitdate THEN l_suppkey ELSE NULL END ) late_suppkey,
                            COUNT(DISTINCT (CASE WHEN l_receiptdate > l_commitdate THEN l_suppkey ELSE NULL END)) AS late_suppliers,
                            COUNT(DISTINCT l_suppkey) AS all_suppliers
                     FROM lineitem
                     GROUP BY l_orderkey)
SELECT s_name, COUNT(*) as numwait
FROM partial_agg
         JOIN supplier ON s_suppkey = late_suppkey
         JOIN orders ON o_orderkey = l_orderkey
         JOIN nation ON supplier.s_nationkey = nation.n_nationkey
WHERE o_orderstatus = 'F' AND n_name = 'ARGENTINA' AND late_suppliers = 1 AND all_suppliers > 1
GROUP BY s_name
ORDER BY numwait DESC, s_name
LIMIT 100