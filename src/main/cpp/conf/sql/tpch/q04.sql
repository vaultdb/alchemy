SELECT o_orderpriority, COUNT(*) as order_count 
FROM  orders JOIN lineitem ON l_orderkey = o_orderkey 
WHERE o_orderdate >= date '1993-07-01' AND o_orderdate < date '1993-10-01' AND l_commitdate < l_receiptdate
GROUP BY  o_orderpriority
ORDER BY o_orderpriority