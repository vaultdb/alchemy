-- 0, collation: (0 ASC)
SELECT c_custkey, c_nationkey
FROM customer JOIN nation ON c_nationkey = n_nationkey
JOIN region ON n_regionkey = r_regionkey
WHERE r_name = 'EUROPE'
ORDER BY c_custkey
-- 1, collation: (1 ASC)
SELECT o_orderkey, o_custkey, NOT (o_orderdate >= DATE '1993-01-01' AND o_orderdate < DATE '1994-01-01') AS dummy_tag
FROM orders
ORDER BY o_custkey;
-- 4, collation: (0 ASC)
SELECT l_orderkey, l_suppkey, l_extendedprice * (1 - l_discount) AS revenue
FROM lineitem
ORDER BY l_orderkey
-- 7, collation: (1 ASC, 0 ASC)
SELECT t2.s_suppkey, t2.s_nationkey FROM
                                        (SELECT r_regionkey, r_name FROM region WHERE r_name = 'EUROPE') AS t0
                                            INNER JOIN
                                            (SELECT n_nationkey, n_regionkey FROM nation) AS t1 ON t0.r_regionkey = t1.n_regionkey
                                            INNER JOIN
                                            (SELECT s_suppkey, s_nationkey FROM supplier) AS t2 ON t1.n_nationkey = t2.s_nationkey
                                    ORDER BY t2.s_nationkey, t2.s_suppkey
