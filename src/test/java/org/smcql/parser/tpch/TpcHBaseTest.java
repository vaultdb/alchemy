package org.smcql.parser.tpch;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;

import org.apache.calcite.rel.RelNode;
import org.smcql.BaseTest;
import org.smcql.config.SystemConfiguration;
import org.smcql.db.data.QueryTable;
import org.smcql.executor.config.ConnectionManager;
import org.smcql.executor.plaintext.SqlQueryExecutor;
import org.smcql.parser.SqlStatementParser;
import org.smcql.type.SecureRelRecordType;
import org.smcql.util.Utilities;

import com.google.common.collect.ImmutableList;

public class TpcHBaseTest extends BaseTest {


	protected static final List<String> QUERIES = ImmutableList.of(
		      // 01
		      "with lineitem_projection as (select l_returnflag, l_linestatus, l_quantity, \n" +
					  "l_extendedprice,l_discount, l_tax,\n" +
					  "l_shipdate\n" +
					  "\n" +
					  "from lineitem) \n" +
					  "\n" +
					  " \n" +
					  "\n" +
					  "select\n" +
					  "l_returnflag,\n" +
					  "l_linestatus,\n" +
					  "sum(l_quantity) as sum_qty,\n" +
					  "sum(l_extendedprice) as sum_base_price,\n" +
					  "sum(l_extendedprice * (1 - l_discount)) as sum_disc_price,\n" +
					  "sum(l_extendedprice * (1 - l_discount) * (1 + l_tax)) as sum_charge,\n" +
					  "avg(l_quantity) as avg_qty,\n" +
					  "avg(l_extendedprice) as avg_price,\n" +
					  "avg(l_discount) as avg_disc,\n" +
					  "count(*) as count_order\n" +
					  "from\n" +
					  "lineitem_projection\n" +
					  "where\n" +
					  "l_shipdate <= date '1998-08-03'\n" +
					  "group by\n" +
					  "l_returnflag,\n" +
					  "l_linestatus\n" +
					  "\n" +
					  "order by\n" +
					  "l_returnflag,\n" +
					  "l_linestatus",

		      // 02
		      "WITH min_ps_supplycost AS (    select\n" +
					  "      ps.ps_partkey,min(ps.ps_supplycost) min_cost\n" +
					  "\n" +
					  "    from\n" +
					  "      partsupp ps,\n" +
					  "      supplier s,\n" +
					  "      nation n,\n" +
					  "      region r\n" +
					  "    where\n" +
					  "      s.s_suppkey = ps.ps_suppkey\n" +
					  "      and s.s_nationkey = n.n_nationkey\n" +
					  "      and n.n_regionkey = r.r_regionkey\n" +
					  "      and r.r_name = 'EUROPE'\n" +
					  "    GROUP BY ps.ps_partkey  ),\n" +
					  "\n" +
					  "parts_projected as (select p_partkey,p_mfgr,p_size,p_type from part where \n" +
					  "  p_type like '%NICKEL' and p_size = 41)\n" +
					  "\n" +
					  "select\n" +
					  "  s.s_acctbal,\n" +
					  "  s.s_name,\n" +
					  "  n.n_name,\n" +
					  "  p.p_partkey,\n" +
					  "  p.p_mfgr,\n" +
					  "  s.s_address,\n" +
					  "  s.s_phone,\n" +
					  "  s.s_comment\n" +
					  "from\n" +
					  "  parts_projected p,\n" +
					  "  supplier s,\n" +
					  "  partsupp ps,\n" +
					  "  nation n,\n" +
					  "  region r,\n" +
					  "  min_ps_supplycost mps\n" +
					  "where\n" +
					  "  p.p_partkey = ps.ps_partkey\n" +
					  "  and s.s_suppkey = ps.ps_suppkey\n" +
					  "\n" +
					  "  and s.s_nationkey = n.n_nationkey\n" +
					  "  and n.n_regionkey = r.r_regionkey\n" +
					  "  and mps.ps_partkey = ps.ps_partkey\n" +
					  "  and ps.ps_supplycost = mps.min_cost\n" +
					  "\n" +
					  "order by\n" +
					  "  s.s_acctbal desc,\n" +
					  "  n.n_name,\n" +
					  "  s.s_name,\n" +
					  "  p.p_partkey\n" +
					  "\n" +
					  "\n",

		      // 03
		      "with customer_proj as (select c_mktsegment, c_custkey from customer\n" +
					  "\n" +
					  " where c_mktsegment= 'HOUSEHOLD' ),\n" +
					  "\n" +
					  "lineitem_proj as (select l_orderkey, l_shipdate,l_extendedprice,\n" +
					  "          l_discount\n" +
					  "          from lineitem where l_shipdate> date '1995-03-25'),\n" +
					  "          \n" +
					  "orders_proj as (select o_custkey,o_orderkey, o_orderdate,o_shippriority from orders \n" +
					  "        \n" +
					  "        where o_orderdate < date '1995-03-25')\n" +
					  "\n" +
					  "\n" +
					  "\n" +
					  "\n" +
					  "select\n" +
					  "  l.l_orderkey,\n" +
					  "  sum(l.l_extendedprice * (1 - l.l_discount)) as revenue,\n" +
					  "  o.o_orderdate,\n" +
					  "  o.o_shippriority\n" +
					  "\n" +
					  "from\n" +
					  "  customer_proj c,\n" +
					  "  orders_proj o,\n" +
					  "  lineitem_proj l\n" +
					  "\n" +
					  "where\n" +
					  "   c.c_custkey = o.o_custkey\n" +
					  "  and l.l_orderkey = o.o_orderkey\n" +
					  "\n" +
					  "  \n" +
					  "\n" +
					  "group by\n" +
					  "  l.l_orderkey,\n" +
					  "  o.o_orderdate,\n" +
					  "  o.o_shippriority\n" +
					  "order by\n" +
					  "  revenue desc,\n" +
					  "  o.o_orderdate limit 10",

		      // 04
		          "select\n"
		          + "  o_orderpriority,\n"
		          + "  count(*) as order_count\n"
		          + "from\n"
		          + "  orders	LEFT JOIN (SELECT DISTINCT l_orderkey FROM lineitem WHERE l_commitdate < l_receiptdate) li ON l_orderkey = o_orderkey\n"
		          + "where o_orderdate >= date '1996-10-01'\n"

		// TODO: address handling date intervals
//		          + "   and o_orderdate < date '1996-10-01' + interval '3 months' \n"
		          + "   and o_orderdate < date '1997-01-01'\n"
		          + "   AND l_orderkey IS NOT NULL\n"
		          + "group by\n"
		          + "  o_orderpriority\n"
		          + "order by\n"
		          + "  o_orderpriority",

		      // 05
		      "with region_proj as (select r_regionkey from region where r_name = 'EUROPE'),\n" +
					  "\n" +
					  "orders_proj as (select o_orderkey,o_custkey from orders where o_orderdate >= date '1997-01-01'\n" +
					  "  and o_orderdate < date '1998-01-01'),\n" +
					  "  \n" +
					  "  lineitem_proj as (select l_orderkey,l_extendedprice ,l_discount, l_suppkey from lineitem),\n" +
					  "  \n" +
					  "  nation_proj as (select n_regionkey,n_nationkey, n_name from nation ),\n" +
					  "  \n" +
					  "  customer_proj as (select c_custkey, c_nationkey from customer),\n" +
					  " \n" +
					  " supplier_proj as (select s_suppkey, s_nationkey from supplier)\n" +
					  " \n" +
					  " \n" +
					  "  \n" +
					  "\n" +
					  "select\n" +
					  "  n.n_name,\n" +
					  "  sum(l.l_extendedprice * (1 - l.l_discount)) as revenue\n" +
					  "\n" +
					  "from\n" +
					  "  customer_proj c,\n" +
					  "  orders_proj o,\n" +
					  "  lineitem_proj l,\n" +
					  "  supplier_proj s,\n" +
					  "  nation_proj n,\n" +
					  "  region_proj r\n" +
					  "\n" +
					  "where\n" +
					  "  c.c_custkey = o.o_custkey\n" +
					  "  and l.l_orderkey = o.o_orderkey\n" +
					  "  and l.l_suppkey = s.s_suppkey\n" +
					  "  and c.c_nationkey = s.s_nationkey\n" +
					  "  and s.s_nationkey = n.n_nationkey\n" +
					  "  and n.n_regionkey = r.r_regionkey\n" +
					  "\n" +
					  "group by\n" +
					  "  n.n_name\n" +
					  "\n" +
					  "order by\n" +
					  "  revenue desc\n" +
					  "\n" +
					  "\n",

		      // 06
		      "with lineitem_proj as (select l_extendedprice, l_discount from lineitem where l_shipdate >= date '1997-01-01'\n" +
					  "  and l_shipdate < date '1998-01-01'\n" +
					  "  and\n" +
					  "  l_discount between 0.03 - 0.01 and 0.03 + 0.01\n" +
					  "  and l_quantity < 24)\n" +
					  "  \n" +
					  "  select sum(l_extendedprice * l_discount) as revenue\n" +
					  "from lineitem_proj\n",

		      // 07
		          "WITH lineitems AS (\n"
		          + "     SELECT l_extendedprice, l_discount, EXTRACT(year from l_shipdate) l_year, l_shipdate, l_suppkey,	l_orderkey\n"
		          + "     FROM lineitem\n"
		          + "     WHERE  l_shipdate >= date '1995-01-01' and l_shipdate <= date '1996-12-31'),\n"
		          + "   shipping AS (\n"
		          + "      select\n"
		          + "      n1.n_name as supp_nation,\n"
		          + "      n2.n_name as cust_nation,\n"
		          + "      l_year,\n"
		          + "      l.l_extendedprice * (1 - l.l_discount) as volume\n"
		          + "    from\n"
		          + "      supplier s,\n"
		          + "      lineitems l,\n"
		          + "      orders o,\n"
		          + "      customer c,\n"
		          + "      nation n1,\n"
		          + "      nation n2\n"
		          + "    where\n"
		          + "      s.s_suppkey = l.l_suppkey\n"
		          + "      and o.o_orderkey = l.l_orderkey\n"
		          + "      and c.c_custkey = o.o_custkey\n"
		          + "      and s.s_nationkey = n1.n_nationkey\n"
		          + "      and c.c_nationkey = n2.n_nationkey\n"
		          + "      and (\n"
		          + "        (n1.n_name = 'EGYPT' and n2.n_name = 'UNITED STATES')\n"
		          + "        or (n1.n_name = 'UNITED STATES' and n2.n_name = 'EGYPT')\n"
		          + "      )\n"
		          + "  ) \n"
		          + "select\n"
		          + "  supp_nation, cust_nation, l_year, sum(volume) as revenue\n"
		          + "from shipping\n"
		          + "group by\n"
		          + "  supp_nation,\n"
		          + "  cust_nation,\n"
		          + "  l_year\n"
		          + "order by\n"
		          + "  supp_nation,\n"
		          + "  cust_nation,\n"
		          + "  l_year",
				          
		          // 08 rewrite
		          "WITH order_year AS (\n"
		          + "     SELECT o.o_orderkey, o.o_custkey, extract(year from o.o_orderdate) as o_year\n"
		          + "     FROM orders o\n"
		          + "     WHERE  o.o_orderdate >= date '1995-01-01'\n"
		          + "       AND  o.o_orderdate <= date '1996-12-31'\n"
		          + "),\n"
		          + "supplier_nation AS (\n"
		          + "		SELECT n_nationkey, n_name, n_name = 'EGYPT' nation_check\n"
		          + "		FROM nation),\n"
		          + "all_nations AS (\n"
		          + "    select\n"
		          + "      o_year,\n"
		          + "      l.l_extendedprice * (1 - l.l_discount) as volume,\n"
		          + "      n2.n_name as nation,\n"
		          + "      nation_check\n"
		          + "    from\n"
		          + "      part p,\n"
		          + "      supplier s,\n"
		          + "      lineitem l,\n"
		          + "      order_year o,\n"
		          + "      customer c,\n"
		          + "      nation n1,\n"
		          + "      supplier_nation n2,\n"
		          + "      region r\n"
		          + "    where\n"
		          + "      p.p_partkey = l.l_partkey\n"
		          + "      and s.s_suppkey = l.l_suppkey\n"
		          + "      and l.l_orderkey = o.o_orderkey\n"
		          + "      and o.o_custkey = c.c_custkey\n"
		          + "      and c.c_nationkey = n1.n_nationkey\n"
		          + "      and n1.n_regionkey = r.r_regionkey\n"
		          + "      and r.r_name = 'MIDDLE EAST'\n"
		          + "      and s.s_nationkey = n2.n_nationkey\n"
		          + "      and p.p_type = 'PROMO BRUSHED COPPER')\n"
		          + "select\n"
		          + "  o_year,\n"
		          + "  sum(case\n"
		          + "    when nation_check then volume\n"
		          + "    else 0\n"
		          + "  end) / sum(volume) as mkt_share\n"
		          + "from all_nations\n"
		          + "group by\n"
		          + "  o_year\n"
		          + "order by\n"
		          + "  o_year\n",

		          // 09
		          "WITH order_years AS (\n"
		          + "     SELECT extract(year from o_orderdate) as o_year, o.o_orderkey\n"
		          + "     FROM orders o),\n"
		          + "     yellow_parts AS (\n"
		          + "         SELECT p_partkey\n"
		          + "	 FROM part\n"
		          + "	 WHERE p_name like '%yellow%'),\n"
		          + "     profit AS (\n"
		          + "         select\n"
		          + "    	   n_name as nation,\n"
		          + "     	    o_year,\n"
		          + "      	    l.l_extendedprice * (1 - l.l_discount) - ps.ps_supplycost * l.l_quantity as amount\n"
		          + "    	  from\n"
		          + "    	    yellow_parts p,\n"
		          + "      	    supplier s,\n"
		          + "      	    lineitem l,\n"
		          + "      	    partsupp ps,\n"
		          + "      	    order_years o,\n"
		          + "      	    nation n\n"
		          + "    where\n"
		          + "      s.s_suppkey = l.l_suppkey\n"
		          + "      and ps.ps_suppkey = l.l_suppkey\n"
		          + "      and ps.ps_partkey = l.l_partkey\n"
		          + "      and p.p_partkey = l.l_partkey\n"
		          + "      and o.o_orderkey = l.l_orderkey\n"
		          + "      and s.s_nationkey = n.n_nationkey)\n"
		          + "select\n"
		          + "  nation,\n"
		          + "  o_year,\n"
		          + "  sum(amount) as sum_profit\n"
		          + "from profit\n"
		          + "group by\n"
		          + "  nation,\n"
		          + "  o_year\n"
		          + "order by\n"
		          + "  nation,\n"
		          + "  o_year desc\n",

		      // 10
		      "select\n"
		          + "  c.c_custkey,\n"
		          + "  c.c_name,\n"
		          + "  sum(l.l_extendedprice * (1 - l.l_discount)) as revenue,\n"
		          + "  c.c_acctbal,\n"
		          + "  n.n_name,\n"
		          + "  c.c_address,\n"
		          + "  c.c_phone,\n"
		          + "  c.c_comment\n"
		          + "from\n"
		          + "  customer c,\n"
		          + "  orders o,\n"
		          + "  lineitem l,\n"
		          + "  nation n\n"
		          + "where\n"
		          + "  c.c_custkey = o.o_custkey\n"
		          + "  and l.l_orderkey = o.o_orderkey\n"
		          + "  and o.o_orderdate >= date '1994-03-01'\n"
		          + "  and o.o_orderdate < date '1994-06-01'\n"
		          + "  and l.l_returnflag = 'R'\n"
		          + "  and c.c_nationkey = n.n_nationkey\n"
		          + "group by\n"
		          + "  c.c_custkey,\n"
		          + "  c.c_name,\n"
		          + "  c.c_acctbal,\n"
		          + "  c.c_phone,\n"
		          + "  n.n_name,\n"
		          + "  c.c_address,\n"
		          + "  c.c_comment\n"
		          + "order by\n"
		          + "  revenue desc\n"
		          + "limit 20",

		      // 11
		      "with nation_proj as (select n_nationkey from nation where n_name = 'JAPAN'),\n" +
					  "\n" +
					  "partsupp_proj as (select ps_partkey,ps_supplycost,ps_availqty, \n" +
					  "\t\t\t\tps_suppkey from partsupp  ),\n" +
					  "\t\t\t\t\n" +
					  "supplier_proj as (select s_nationkey,s_suppkey  from supplier )\n" +
					  "\n" +
					  "\n" +
					  "select\n" +
					  "  ps.ps_partkey,\n" +
					  "  sum(ps.ps_supplycost * ps.ps_availqty) as val\n" +
					  "from\n" +
					  "  partsupp_proj ps,\n" +
					  "  supplier_proj s,\n" +
					  "  nation_proj n\n" +
					  "where\n" +
					  "  ps.ps_suppkey = s.s_suppkey\n" +
					  "  and s.s_nationkey = n.n_nationkey\n" +
					  "  \n" +
					  "group by\n" +
					  "  ps.ps_partkey having\n" +
					  "    sum(ps.ps_supplycost * ps.ps_availqty) > (\n" +
					  "      select\n" +
					  "        sum(ps.ps_supplycost * ps.ps_availqty) * 0.0001000000\n" +
					  "      from\n" +
					  "        partsupp ps,\n" +
					  "        supplier s,\n" +
					  "        nation n\n" +
					  "      where\n" +
					  "        ps.ps_suppkey = s.s_suppkey\n" +
					  "        and s.s_nationkey = n.n_nationkey\n" +
					  "        and n.n_name = 'JAPAN'\n" +
					  "    )\n" +
					  "order by\n" +
					  "  val desc\n" +
					  "\n" ,

		      // 12
		      "with order_proj as (select o_orderkey, o_orderpriority \n" +
					  "  from orders \n" +
					  "  \n" +
					  "),\n" +
					  "\n" +
					  "lineitem_proj as (select l_shipmode, l_orderkey \n" +
					  "  from lineitem \n" +
					  "  where \n" +
					  "l_shipmode in ('TRUCK', 'REG AIR')\n" +
					  "and l_commitdate < l_receiptdate\n" +
					  "  and l_shipdate < l_commitdate\n" +
					  "  and l_receiptdate >= date '1994-01-01'\n" +
					  "  and l_receiptdate < date '1995-01-01'\n" +
					  "\n" +
					  " )\n" +
					  "\n" +
					  "select\n" +
					  "  l.l_shipmode,\n" +
					  "  sum(case\n" +
					  "    when o.o_orderpriority = '1-URGENT'\n" +
					  "      or o.o_orderpriority = '2-HIGH'\n" +
					  "      then 1\n" +
					  "    else 0\n" +
					  "  end) as high_line_count,\n" +
					  "  \n" +
					  "  sum(case\n" +
					  "    when o.o_orderpriority <> '1-URGENT'\n" +
					  "      and o.o_orderpriority <> '2-HIGH'\n" +
					  "      then 1\n" +
					  "    else 0\n" +
					  "  end) as low_line_count\n" +
					  "from\n" +
					  "  order_proj o,\n" +
					  "  lineitem_proj l\n" +
					  "where\n" +
					  "  o.o_orderkey = l.l_orderkey\n" +
					  "  \n" +
					  "group by\n" +
					  "  l.l_shipmode\n" +
					  "order by\n" +
					  "  l.l_shipmode\n",



		      // 13
		          "WITH tagged_orders AS (\n"
		          + "     SELECT o_custkey, o_orderkey\n"
		          + "     FROM orders o\n"
		          + "     WHERE o.o_comment not like '%special%requests%'),\n"
		          + "   customer_orders AS (    \n"
		          + "     select c.c_custkey, count(o.o_orderkey) c_count\n"
		          + "    from customer c  left outer join tagged_orders o \n"
		          + "        on c.c_custkey = o.o_custkey\n"
		          + "    group by\n"
		          + "      c.c_custkey  )\n"
		          + " select\n"
		          + "  c_count,\n"
		          + "  count(*) as custdist\n"
		          + "from\n"
		          + "  customer_orders\n"
		          + "group by\n"
		          + "  c_count\n"
		          + "order by\n"
		          + "  custdist desc,\n"
		          + "  c_count desc",		          

		
			// 14
		      "with lineitem_proj as (select l_partkey, l_extendedprice, l_discount from lineitem where\n" +
					  "l_shipdate >= date '1994-08-01'\n" +
					  "  and l_shipdate < date '1994-09-01'\n" +
					  " )\n" +
					  "\n" +
					  "select\n" +
					  "  100.00 * sum(case\n" +
					  "    when p.p_type like 'PROMO%'\n" +
					  "      then l.l_extendedprice * (1 - l.l_discount)\n" +
					  "    else 0\n" +
					  "  end) / sum(l.l_extendedprice * (1 - l.l_discount)) as promo_revenue\n" +
					  "from\n" +
					  "  lineitem_proj l,\n" +
					  "  part p\n" +
					  "where\n" +
					  "  l.l_partkey = p.p_partkey\n" +
					  " ",

		      // 15
		      "with revenue0 (supplier_no, total_revenue) as (\n" +
					  "  select\n" +
					  "    l_suppkey,\n" +
					  "    sum(l_extendedprice * (1 - l_discount))\n" +
					  "  from\n" +
					  "    lineitem\n" +
					  "  where\n" +
					  "    l_shipdate >= date '1993-05-01'\n" +
					  "    and l_shipdate < date '1993-08-01'\n" +
					  "  group by\n" +
					  "    l_suppkey),\n" +
					  "max_rev_proj as (select\n" +
					  "      max(total_revenue) as max_rev\n" +
					  "    from\n" +
					  "      revenue0)\n" +
					  "\n" +
					  "select\n" +
					  "  s.s_suppkey,\n" +
					  "  s.s_name,\n" +
					  "  s.s_address,\n" +
					  "  s.s_phone,\n" +
					  "  r.total_revenue\n" +
					  "from\n" +
					  "  supplier s,\n" +
					  "  revenue0 r,\n" +
					  "    max_rev_proj\n" +
					  "where\n" +
					  "  s.s_suppkey = r.supplier_no\n" +
					  "  and r.total_revenue = max_rev_proj.max_rev\n" +
					  "order by\n" +
					  "  s.s_suppkey\n",

		      // 16 - Potentially indicating to look at string replacements
		         // TODO: fix compiler with commented lines.  
		          // these can all be handled in SqlGenerator
		          // hence needing to make sure that the new CalciteTest
		          // SQL --> RelNode --> SQL works correctly
		          // also, re-add  subquery
		          
		          // The key here will be to push down all filters and projects that have expressions 
		          // so that we don't hit the SMC parser for all of these specialized expression like 
		          // EXTRACT(Year FROM ...)
	
		          "WITH complaints AS (SELECT * FROM supplier WHERE s_comment LIKE '%Customer%Complaints%'),\n" +
						  "     ps_suppkey_set_diff AS (\n" +
						  "     SELECT ps_partkey, ps_suppkey\n" +
						  "     FROM partsupp ps LEFT JOIN complaints c ON ps.ps_suppkey = c.s_suppkey\n" +
						  "     WHERE c.s_suppkey IS NULL),\n" +
						  "\n" +
						  "     part_pro  AS ( select p_partkey, p_brand,p_type,p_size from part p \n" +
						  "      where\n" +
						  "      p.p_brand <> 'Brand#21'\n" +
						  "      and p.p_type not like 'MEDIUM PLATED%'\n" +
						  "        AND p.p_size in (38, 2, 8, 31, 44, 5, 14, 24)\n" +
						  "\n" +
						  "      )\n" +
						  "\n" +
						  "select\n" +
						  "   p.p_brand,\n" +
						  "   p.p_type,\n" +
						  "   p.p_size,\n" +
						  "   count(distinct ps.ps_suppkey) as supplier_cnt\n" +
						  " from\n" +
						  "   ps_suppkey_set_diff ps,\n" +
						  "   part_pro p\n" +
						  " where\n" +
						  "   p.p_partkey = ps.ps_partkey\n" +
						  "   \n" +
						  " group by\n" +
						  "   p.p_brand,\n" +
						  "   p.p_type,\n" +
						  "   p.p_size\n" +
						  " order by\n" +
						  "   supplier_cnt desc,\n" +
						  "   p.p_brand,\n" +
						  "   p.p_type,\n" +
						  "   p.p_size",

		      // 17
		      "WITH l2table AS (\n" +
					  "  select \n" +
					  "    l_partkey, 0.2 * avg(l2.l_quantity) AS avg_l2_q\n" +
					  "  from\n" +
					  "    lineitem l2\n" +
					  "  group by\n" +
					  "    l2.l_partkey\n" +
					  "),\n" +
					  "\n" +
					  "part_proj as (select p_partkey from part \n" +
					  "where \n" +
					  "p_container = 'JUMBO CAN'\n" +
					  "and\n" +
					  "p_brand = 'Brand#13'\n" +
					  ")\n" +
					  "select\n" +
					  "  sum(l.l_extendedprice) / 7.0 as avg_yearly\n" +
					  "from\n" +
					  "  lineitem l,\n" +
					  "  part_proj p,\n" +
					  "  l2table l2\n" +
					  "where\n" +
					  "  p.p_partkey = l.l_partkey\n" +
					  "  \n" +
					  "  and l.l_quantity < l2.avg_l2_q\n" +
					  "  and l2.l_partkey = p.p_partkey",

			// 18
		      "select\n"
		          + "  c.c_name,\n"
		          + "  c.c_custkey,\n"
		          + "  o.o_orderkey,\n"
		          + "  o.o_orderdate,\n"
		          + "  o.o_totalprice,\n"
		          + "  sum(l.l_quantity)\n"
		          + "from\n"
		          + "  customer c,\n"
		          + "  orders o,\n"
		          + "  lineitem l\n"
		          + "where\n"
		          + "  o.o_orderkey in (\n"
		          + "    select\n"
		          + "      l_orderkey\n"
		          + "    from\n"
		          + "      lineitem\n"
		          + "    group by\n"
		          + "      l_orderkey having\n"
		          + "        sum(l_quantity) > 313\n"
		          + "  )\n"
		          + "  and c.c_custkey = o.o_custkey\n"
		          + "  and o.o_orderkey = l.l_orderkey\n"
		          + "group by\n"
		          + "  c.c_name,\n"
		          + "  c.c_custkey,\n"
		          + "  o.o_orderkey,\n"
		          + "  o.o_orderdate,\n"
		          + "  o.o_totalprice\n"
		          + "order by\n"
		          + "  o.o_totalprice desc,\n"
		          + "  o.o_orderdate\n"
		          + "limit 100",

		      // 19
		      "select\n"
		          + "  sum(l.l_extendedprice* (1 - l.l_discount)) as revenue\n"
		          + "from\n"
		          + "  lineitem l,\n"
		          + "  part p\n"
		          + "where\n"
		          + "  (\n"
		          + "    p.p_partkey = l.l_partkey\n"
		          + "    and p.p_brand = 'Brand#41'\n"
		          + "    and p.p_container in ('SM CASE', 'SM BOX', 'SM PACK', 'SM PKG')\n"
		          + "    and l.l_quantity >= 2 and l.l_quantity <= 2 + 10\n"
		          + "    and p.p_size between 1 and 5\n"
		          + "    and l.l_shipmode in ('AIR', 'AIR REG')\n"
		          + "    and l.l_shipinstruct = 'DELIVER IN PERSON'\n"
		          + "  )\n"
		          + "  or\n"
		          + "  (\n"
		          + "    p.p_partkey = l.l_partkey\n"
		          + "    and p.p_brand = 'Brand#13'\n"
		          + "    and p.p_container in ('MED BAG', 'MED BOX', 'MED PKG', 'MED PACK')\n"
		          + "    and l.l_quantity >= 14 and l.l_quantity <= 14 + 10\n"
		          + "    and p.p_size between 1 and 10\n"
		          + "    and l.l_shipmode in ('AIR', 'AIR REG')\n"
		          + "    and l.l_shipinstruct = 'DELIVER IN PERSON'\n"
		          + "  )\n"
		          + "  or\n"
		          + "  (\n"
		          + "    p.p_partkey = l.l_partkey\n"
		          + "    and p.p_brand = 'Brand#55'\n"
		          + "    and p.p_container in ('LG CASE', 'LG BOX', 'LG PACK', 'LG PKG')\n"
		          + "    and l.l_quantity >= 23 and l.l_quantity <= 23 + 10\n"
		          + "    and p.p_size between 1 and 15\n"
		          + "    and l.l_shipmode in ('AIR', 'AIR REG')\n"
		          + "    and l.l_shipinstruct = 'DELIVER IN PERSON'\n"
		          + "  )",

		      // 20
		          "WITH antiques AS (select DISTINCT p.p_partkey\n"
		          + "                                 from\n"
		          + "                                   part p\n"
		          + "                                 where\n"
		          + "                                   p.p_name like 'antique%'\n"
		          + "                                ),\n"
		          + "            partial_agg AS (\n"
		          + "                            select\n"
		          + "                                   l_partkey, l_suppkey, 0.5 * sum(l.l_quantity) inventory\n"
		          + "                                 from\n"
		          + "                                   lineitem l\n"
		          + "                                 where\n"
		          + "                                    l.l_shipdate >= date '1993-01-01'\n"
		          + "                                   and l.l_shipdate < date '1994-01-01'\n"
		          + "                              GROUP BY l_partkey, l_suppkey\n"
		          + "                     ),\n"
		          + "\n"
		          + "            qualified AS (\n"
		          + "                          select DISTINCT ps.ps_suppkey\n"
		          + "                          from\n"
		          + "                            partial_agg pa,\n"
		          + "                            partsupp ps LEFT JOIN antiques a ON ps.ps_partkey = a.p_partkey\n"
		          + "                          WHERE a.p_partkey IS NOT NULL\n"
		          + "                            and ps.ps_availqty > pa.inventory AND pa.l_partkey = ps.ps_partkey AND pa.l_suppkey = ps.ps_suppkey\n"
		          + "                            )\n"
		          + "select\n"
		          + "                        s.s_name,\n"
		          + "                        s.s_address\n"
		          + "                      from\n"
		          + "                        supplier s LEFT JOIN qualified q ON s.s_suppkey = q.ps_suppkey,\n"
		          + "                        nation n\n"
		          + "                      where\n"
		          + "                        ps_suppkey IS NOT NULL\n"
		          + "                        and s.s_nationkey = n.n_nationkey\n"
		          + "                        and n.n_name = 'KENYA'\n"
		          + "                      order by\n"
		          + "                        s.s_name\n",

					//21
					" WITH l2 AS ( select DISTINCT l_suppkey, l_orderkey\n"
					+ "                                              from\n"
					+ "                                                lineitem l1),\n"
					+ "\n"
					+ "\n"
					+ " l1prime AS (SELECT DISTINCT l1.l_suppkey, l1.l_orderkey \n"
					+ "   FROM lineitem l1 LEFT JOIN l2 ON (l2.l_orderkey = l1.l_orderkey\n"
					+ " AND l2.l_suppkey <> l1.l_suppkey)\n"
					+ "                                       WHERE l2.l_orderkey IS NOT NULL),\n"
					+ "\n"
					+ "\n"
					+ "  l3 AS (\n"
					+ "       select DISTINCT  l_orderkey, l_suppkey\n"
					+ "      from lineitem l3\n"
					+ "      where l3.l_receiptdate > l3.l_commitdate\n"
					+ "   ),\n"
					+ "\n"
					+ "  l1doubleprime AS (SELECT l1prime.l_suppkey, l1prime.l_orderkey, l1prime.l_receiptdate, l1prime.l_commitdate\n"
					+ "     FROM lineitem l1prime LEFT JOIN l3 ON l3.l_orderkey = l1prime.l_orderkey  and l3.l_suppkey <> l1prime.l_suppkey\n"
					+ "     WHERE  l3.l_orderkey IS NULL),\n"
					+ "\n"
					+ " agg AS (select\n"
					+ "    s.s_name, l_orderkey, l_suppkey\n"
					+ "  from\n"
					+ "    supplier s,\n"
					+ "    l1doubleprime l1,\n"
					+ "    orders o,\n"
					+ "    nation n\n"
					+ "  where\n"
					+ "    s.s_suppkey = l1.l_suppkey\n"
					+ "    and o.o_orderkey = l1.l_orderkey\n"
					+ "    and o.o_orderstatus = 'F'\n"
					+ "    and l1.l_receiptdate > l1.l_commitdate\n"
					+ "    and s.s_nationkey = n.n_nationkey\n"
					+ "    and n.n_name = 'BRAZIL'\n"
					+ "  order by\n"
					+ "    s.s_name)\n"
					+ "\n"
					+ " SELECT agg.s_name, count(*) as numwait FROM agg LEFT JOIN l1prime on agg.l_orderkey = l1prime.l_orderkey and agg.l_suppkey = l1prime.l_suppkey WHERE l1prime.l_suppkey IS NOT NULL\n"
					+ " group by\n"
					+ "   agg.s_name\n"
					+ " order by\n"
					+ "   numwait desc,\n"
					+ "   agg.s_name\n"
					+ " limit 100",




		      // 22
			  "WITH\n"
				+ "	  c_sub\n"
				+ "	  AS (\n"
				+ "	  		SELECT\n"
				+ "	  		substring(c_phone, 1, 2) AS cntrycode, *\n"
				+ "	  		FROM\n"
				+ "	  customer AS c\n"
				+ "	  WHERE\n"
				+ "	  substring(c_phone, 1, 2)\n"
				+ "	  IN ('24','31','11','16','21','20','34')\n"
				+ "    ),\n"
				+ "	  		c_avg\n"
				+ "	  AS (\n"
				+ "	  		SELECT\n"
				+ "	  				avg(c_acctbal) AS avg_c_acctbal\n"
				+ "	  FROM\n"
				+ "	  		customer\n"
				+ "	  WHERE\n"
				+ "	  c_acctbal > 0.00\n"
				+ "	  AND substring(c_phone, 1, 2)\n"
				+ "	  IN ('24','31','11','16','21','20','34')\n"
				+ "	  	),\n"
				+ "	  c_avg_restrict\n"
				+ "	  AS (\n"
				+ "	  		SELECT\n"
				+ "	  				c.*, a.avg_c_acctbal\n"
				+ "	  				FROM\n"
				+ "	  			c_sub AS c, c_avg AS a\n"
				+ "	  				WHERE\n"
				+ "	  				c.c_acctbal > a.avg_c_acctbal\n"
				+ "	  ),\n"
				+ "	  c_orderkey\n"
				+ "	  AS (\n"
				+ "	  		SELECT\n"
				+ "	  			*\n"
				+ "	  					FROM\n"
				+ "	  					c_avg_restrict AS c\n"
				+ "	  					LEFT JOIN orders AS o ON\n"
				+ "	  					o.o_custkey = c.c_custkey\n"
				+ "	  					WHERE\n"
				+ "	  					o.o_custkey IS NULL\n"
				+ "	  )\n"
				+ "	  SELECT\n"
				+ "	  		cntrycode,\n"
				+ "	  count(*) AS numcust,\n"
				+ "	  sum(c_acctbal) AS totacctbal\n"
				+ "	  FROM\n"
				+ "	  c_orderkey AS c\n"
				+ "	  GROUP BY\n"
				+ "	  cntrycode\n"
				+ "	  ORDER BY\n"
				+ "	  cntrycode",

			//QUERY 23 for 13
			"    select\n"
					+ "      c.c_custkey,\n"
					+ "      count(o.o_orderkey)\n"
					+ "    from\n"
					+ "      customer c \n"
					+ "      left outer join orders o \n"
					+ "        on c.c_custkey = o.o_custkey\n"
					+ "        WHERE  o.o_comment not like '%special%requests%'\n"
					+ "    group by\n"
					+ "      c.c_custkey\n"




	);


	  protected void setUp() throws Exception {

		    String setupFile = Utilities.getSMCQLRoot() + "/conf/setup.tpch";
		    System.setProperty("smcql.setup", setupFile);
		 	logger = SystemConfiguration.getInstance().getLogger();
		 	parser = new SqlStatementParser();


	  }

	  // runs query over unioned database
	  // the sql statement is written against the shared schema
	  protected QueryTable getExpectedOutput(String sql, SecureRelRecordType outSchema) throws Exception {
		  String unionedId = ConnectionManager.getInstance().getUnioned();
		  return SqlQueryExecutor.query(sql, outSchema, unionedId);
	  }

	public QueryTable getExpectedDistributedOutput(String sql, SecureRelRecordType outSchema) throws Exception{
		String aliceId = ConnectionManager.getInstance().getAlice();
		return SqlQueryExecutor.query(sql, outSchema, aliceId);
	}



}
