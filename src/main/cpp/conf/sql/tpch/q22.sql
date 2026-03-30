SELECT
     c_phone,
     COUNT(*) AS numcust,
     SUM(c_acctbal) AS totacctbal
 FROM (
          SELECT
              SUBSTRING(c_phone FROM 1 FOR 2)::varchar AS c_phone,
              c_acctbal
          FROM
              customer
          WHERE
                  SUBSTRING(c_phone FROM 1 FOR 2) IN
                  ('24', '31', '11', '16', '21', '20', '34')
            AND c_acctbal > (
              SELECT
                  AVG(c_acctbal)
              FROM
                  customer
              WHERE
                      c_acctbal > 0.00
                AND SUBSTRING(c_phone FROM 1 FOR 2) IN
                    ('24', '31', '11', '16', '21', '20', '34')
          )
            AND NOT EXISTS (
                  SELECT
                      *
                  FROM
                      orders
                  WHERE
                          o_custkey = c_custkey
              )
      ) AS custsale
 GROUP BY
    c_phone
 ORDER BY
    c_phone;