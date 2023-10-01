-- 0, collation: (0 ASC), party: 1
WITH all_keys AS (SELECT o_orderkey AS ak FROM order_keys),
     sum_qtys AS (SELECT l_orderkey, sum(l_quantity) as sum_qty, false AS dummy_tag
                  FROM lineitem
                  GROUP BY l_orderkey
                  ORDER BY l_orderkey)
SELECT ak, COALESCE(sum_qty, 0.0) sum_qty, COALESCE(dummy_tag, true) dummy_tag
FROM all_keys LEFT JOIN sum_qtys ON l_orderkey = ak
ORDER BY ak;
-- 1, collation: (0 ASC), party: 2
WITH all_keys AS (SELECT o_orderkey AS ak FROM order_keys),
     sum_qtys AS (SELECT l_orderkey, sum(l_quantity) as sum_qty, false AS dummy_tag
                  FROM lineitem
                  GROUP BY l_orderkey
                  ORDER BY l_orderkey)
SELECT ak, COALESCE(sum_qty, 0.0) sum_qty, COALESCE(dummy_tag, true) dummy_tag
FROM all_keys LEFT JOIN sum_qtys ON l_orderkey = ak
ORDER BY ak;
-- 5, collation: (0 ASC)
SELECT o_orderkey, o_custkey, o_totalprice, o_orderdate
FROM orders
ORDER BY o_orderkey
-- 8
SELECT c_custkey, c_name
FROM customer
ORDER BY c_custkey