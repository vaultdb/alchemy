-- 0, collation: (0 ASC, 1 ASC), party: 1
WITH output_domain AS (
    SELECT  *
    FROM (VALUES('R'::char(1), 'F'::char(1)), ('A', 'F'), ('N', 'F'), ('R', 'O'), ('A', 'O'), ('N', 'O')) AS li(l_returnflag, l_linestatus)
    ORDER BY l_returnflag, l_linestatus),
    partial_aggs AS (
        SELECT l_returnflag, l_linestatus, SUM(l_quantity) sum_qty, SUM(l_extendedprice) sum_base_price,
              SUM(l_extendedprice*(1-l_discount)) sum_disc_price, SUM(l_extendedprice*(1-l_discount)*(1+l_tax)) sum_charge,
              SUM(l_quantity) avg_qty_sum, COUNT(l_quantity)::FLOAT4 avg_qty_cnt,
              SUM(l_extendedprice) avg_price_sum, COUNT(l_extendedprice)::FLOAT4 avg_price_cnt,
              SUM(l_discount) avg_discount_sum, COUNT(l_discount)::FLOAT4 avg_discount_cnt, COUNT(*) cnt
        FROM lineitem
        WHERE l_shipdate  <= date '1998-08-03'
        GROUP BY l_returnflag, l_linestatus
        ORDER BY l_returnflag, l_linestatus)
SELECT o.l_returnflag, o.l_linestatus, COALESCE(sum_qty, 0) sum_qty, COALESCE(sum_base_price, 0) sum_base_price, COALESCE(sum_disc_price, 0) sum_disc_price, COALESCE(sum_charge, 0) sum_charge, COALESCE(avg_qty_sum, 0) avg_qty_sum, COALESCE(avg_qty_cnt, 0) avg_qty_cnt,  COALESCE(avg_price_sum, 0) avg_price_sum, COALESCE(avg_price_cnt, 0) avg_price_cnt,  COALESCE(avg_discount_sum, 0) avg_discount_sum, COALESCE(avg_discount_cnt, 0) avg_discount_cnt,  COALESCE(cnt, 0)::BIGINT cnt, CASE WHEN p.l_returnflag IS NULL THEN true ELSE false END AS dummy_tag
FROM output_domain o LEFT JOIN partial_aggs p ON o.l_returnflag = p.l_returnflag AND o.l_linestatus = p.l_linestatus
ORDER BY o.l_returnflag, o.l_linestatus;

-- 1, collation: (0 ASC, 1 ASC), party: 2
WITH output_domain AS (
    SELECT  *
    FROM (VALUES('R'::char(1), 'F'::char(1)), ('A', 'F'), ('N', 'F'), ('R', 'O'), ('A', 'O'), ('N', 'O')) AS li(l_returnflag, l_linestatus)
    ORDER BY l_returnflag, l_linestatus),
     partial_aggs AS (
         SELECT l_returnflag, l_linestatus, SUM(l_quantity) sum_qty, SUM(l_extendedprice) sum_base_price,
                SUM(l_extendedprice*(1-l_discount)) sum_disc_price, SUM(l_extendedprice*(1-l_discount)*(1+l_tax)) sum_charge,
                SUM(l_quantity) avg_qty_sum, COUNT(l_quantity)::FLOAT4 avg_qty_cnt,
                SUM(l_extendedprice) avg_price_sum, COUNT(l_extendedprice)::FLOAT4 avg_price_cnt,
                SUM(l_discount) avg_discount_sum, COUNT(l_discount)::FLOAT4 avg_discount_cnt, COUNT(*) cnt
         FROM lineitem
         WHERE l_shipdate  <= date '1998-08-03'
         GROUP BY l_returnflag, l_linestatus
         ORDER BY l_returnflag, l_linestatus)
SELECT o.l_returnflag, o.l_linestatus, COALESCE(sum_qty, 0) sum_qty, COALESCE(sum_base_price, 0) sum_base_price, COALESCE(sum_disc_price, 0) sum_disc_price, COALESCE(sum_charge, 0) sum_charge, COALESCE(avg_qty_sum, 0) avg_qty_sum, COALESCE(avg_qty_cnt, 0) avg_qty_cnt,  COALESCE(avg_price_sum, 0) avg_price_sum, COALESCE(avg_price_cnt, 0) avg_price_cnt,  COALESCE(avg_discount_sum, 0) avg_discount_sum, COALESCE(avg_discount_cnt, 0) avg_discount_cnt,  COALESCE(cnt, 0)::BIGINT cnt, CASE WHEN p.l_returnflag IS NULL THEN true ELSE false END AS dummy_tag
FROM output_domain o LEFT JOIN partial_aggs p ON o.l_returnflag = p.l_returnflag AND o.l_linestatus = p.l_linestatus
ORDER BY o.l_returnflag, o.l_linestatus;
