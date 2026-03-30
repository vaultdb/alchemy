SELECT
    n1.n_name,
    n2.n_name,
    EXTRACT(YEAR FROM l_shipdate)::INT AS l_year,
        SUM(l_extendedprice * (1 - l_discount)) AS revenue
FROM
    supplier,
    lineitem,
    orders,
    customer,
    nation n1,
    nation n2
WHERE
        s_suppkey = l_suppkey
  AND o_orderkey = l_orderkey
  AND c_custkey = o_custkey
  AND s_nationkey = n1.n_nationkey
  AND c_nationkey = n2.n_nationkey
  AND (
        (n1.n_name = 'UNITED STATES' AND n2.n_name = 'EGYPT')
        OR (n1.n_name = 'EGYPT' AND n2.n_name = 'UNITED STATES')
    )
  AND l_shipdate BETWEEN DATE '1995-01-01' AND DATE '1996-12-31'
GROUP BY
    n1.n_name,
    n2.n_name,
    l_year
ORDER BY
    n1.n_name,
    n2.n_name,
    l_year;
