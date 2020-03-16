package org.vaultdb;

import java.util.List;
import java.util.Map;
import java.util.logging.Logger;

import org.apache.calcite.rel.RelRoot;
import org.apache.calcite.sql.SqlDialect;
import org.apache.calcite.sql.SqlNode;
import org.vaultdb.config.SystemConfiguration;
import org.vaultdb.db.data.QueryTable;
import org.vaultdb.db.schema.SystemCatalog;
import org.vaultdb.executor.config.ConnectionManager;
import org.vaultdb.executor.config.WorkerConfiguration;
import org.vaultdb.executor.plaintext.SqlQueryExecutor;
import org.vaultdb.parser.SqlStatementParser;
import org.vaultdb.type.SecureRelRecordType;
import org.vaultdb.util.Utilities;

import com.google.common.collect.ImmutableList;
import com.google.common.collect.ImmutableMap;

import junit.framework.TestCase;

public abstract class TpcHBaseTest  extends TestCase {

	protected SqlStatementParser parser;
	protected SqlNode root;
	protected RelRoot relRoot;
	protected SqlDialect dialect;
	protected String codePath = Utilities.getVaultDBRoot() + "/conf/workload/sql";
	protected WorkerConfiguration honestBroker;
	protected Logger logger;
	protected SystemConfiguration config;
	protected SystemCatalog catalog;

	
	protected static final List<String> QUERIES = ImmutableList.of(
		      // 01
		      "select"
		          + "  l_returnflag,"
		          + "  l_linestatus,"
		          + "  sum(l_quantity) as sum_qty,"
		          + "  sum(l_extendedprice) as sum_base_price,"
		          + "  sum(l_extendedprice * (1 - l_discount)) as sum_disc_price,"
		          + "  sum(l_extendedprice * (1 - l_discount) * (1 + l_tax)) as sum_charge,"
		          + "  avg(l_quantity) as avg_qty,"
		          + "  avg(l_extendedprice) as avg_price,"
		          + "  avg(l_discount) as avg_disc,"
		          + "  count(*) as count_order"
		          + "from"
		          + "  lineitem"
		          + " where"
		          + "  l_shipdate <= date '1998-08-03'"
		          + "group by"
		          + "  l_returnflag,"
		          + "  l_linestatus"
		          + ""
		          + "order by"
		          + "  l_returnflag,"
		          + "  l_linestatus",

		      // 02
		      "WITH min_ps_supplycost AS ("
	          + "    select"
	          + "      ps.ps_partkey,min(ps.ps_supplycost) min_cost"
	          + ""
	          + "    from"
	          + "      partsupp ps,"
	          + "      supplier s,"
	          + "      nation n,"
	          + "      region r"
	          + "    where"
	          + "      s.s_suppkey = ps.ps_suppkey"
	          + "      and s.s_nationkey = n.n_nationkey"
	          + "      and n.n_regionkey = r.r_regionkey"
	          + "      and r.r_name = 'EUROPE'"
	          + "    GROUP BY ps.ps_partkey"
	          + "  )"
		      + "select"
		          + "  s.s_acctbal,"
		          + "  s.s_name,"
		          + "  n.n_name,"
		          + "  p.p_partkey,"
		          + "  p.p_mfgr,"
		          + "  s.s_address,"
		          + "  s.s_phone,"
		          + "  s.s_comment"
		          + "from"
		          + "  part p,"
		          + "  supplier s,"
		          + "  partsupp ps,"
		          + "  nation n,"
		          + "  region r,"
		          + "  min_ps_supplycost mps"
		          + "where"
		          + "  p.p_partkey = ps.ps_partkey"
		          + "  and s.s_suppkey = ps.ps_suppkey"
		          + "  and p.p_size = 41"
		          + "  and p.p_type like '%NICKEL'"
		          + "  and s.s_nationkey = n.n_nationkey"
		          + "  and n.n_regionkey = r.r_regionkey"
		          + "  and r.r_name = 'EUROPE'"
		          + "  and mps.ps_partkey = ps.ps_partkey"
		          + "  and ps.ps_supplycost = mps.min_cost"
		          + ""
		          + "order by"
		          + "  s.s_acctbal desc,"
		          + "  n.n_name,"
		          + "  s.s_name,"
		          + "  p.p_partkey"
		          + "limit 100",

		      // 03
		      "select"
		          + "  l.l_orderkey,"
		          + "  sum(l.l_extendedprice * (1 - l.l_discount)) as revenue,"
		          + "  o.o_orderdate,"
		          + "  o.o_shippriority"
		          + ""
		          + "from"
		          + "  customer c,"
		          + "  orders o,"
		          + "  lineitem l"
		          + ""
		          + "where"
		          + "  c.c_mktsegment = 'HOUSEHOLD'"
		          + "  and c.c_custkey = o.o_custkey"
		          + "  and l.l_orderkey = o.o_orderkey"
		          + "  and o.o_orderdate < date '1995-03-25'"
		          + "  and l.l_shipdate > date '1995-03-25'"
		          + ""
		          + "group by"
		          + "  l.l_orderkey,"
		          + "  o.o_orderdate,"
		          + "  o.o_shippriority"
		          + "order by"
		          + "  revenue desc,"
		          + "  o.o_orderdate"
		          + "limit 10",

		      // 04
		          "SELECT o_orderpriority, COUNT(*) as order_count "
		          + "FROM  orders JOIN lineitem ON l_orderkey = o_orderkey "
		          + "WHERE o_orderdate >= date '1993-07-01' AND o_orderdate < date '1998-12-01' AND l_commitdate < l_receiptdate"
		          + "GROUP BY  o_orderpriority"
		          + "ORDER BY o_orderpriority",

		      // 05
		      "select"
		          + "  n.n_name,"
		          + "  sum(l.l_extendedprice * (1 - l.l_discount)) as revenue"
		          + ""
		          + "from"
		          + "  customer c,"
		          + "  orders o,"
		          + "  lineitem l,"
		          + "  supplier s,"
		          + "  nation n,"
		          + "  region r"
		          + ""
		          + "where"
		          + "  c.c_custkey = o.o_custkey"
		          + "  and l.l_orderkey = o.o_orderkey"
		          + "  and l.l_suppkey = s.s_suppkey"
		          + "  and c.c_nationkey = s.s_nationkey"
		          + "  and s.s_nationkey = n.n_nationkey"
		          + "  and n.n_regionkey = r.r_regionkey"
		          + "  and r.r_name = 'EUROPE'"
		          + "  and o.o_orderdate >= date '1997-01-01'"
		          + "  and o.o_orderdate < date '1998-01-01'"
		          + "group by"
		          + "  n.n_name"
		          + ""
		          + "order by"
		          + "  revenue desc",

		      // 06
		      "select"
		          + "  sum(l_extendedprice * l_discount) as revenue"
		          + "from"
		          + "  lineitem"
		          + "where"
		          + "  l_shipdate >= date '1997-01-01'"
		          + "  and l_shipdate < date '1998-01-01'"
		          + "  and"
		          + "  l_discount between 0.03 - 0.01 and 0.03 + 0.01"
		          + "  and l_quantity < 24",

		      // 07
		          "WITH lineitems AS ("
		          + "     SELECT l_extendedprice, l_discount, EXTRACT(year from l_shipdate) l_year, l_shipdate, l_suppkey,	l_orderkey"
		          + "     FROM lineitem"
		          + "     WHERE  l_shipdate >= date '1995-01-01' and l_shipdate <= date '1996-12-31'),"
		          + "   shipping AS ("
		          + "      select"
		          + "      n1.n_name as supp_nation,"
		          + "      n2.n_name as cust_nation,"
		          + "      l_year,"
		          + "      l.l_extendedprice * (1 - l.l_discount) as volume"
		          + "    from"
		          + "      supplier s,"
		          + "      lineitems l,"
		          + "      orders o,"
		          + "      customer c,"
		          + "      nation n1,"
		          + "      nation n2"
		          + "    where"
		          + "      s.s_suppkey = l.l_suppkey"
		          + "      and o.o_orderkey = l.l_orderkey"
		          + "      and c.c_custkey = o.o_custkey"
		          + "      and s.s_nationkey = n1.n_nationkey"
		          + "      and c.c_nationkey = n2.n_nationkey"
		          + "      and ("
		          + "        (n1.n_name = 'EGYPT' and n2.n_name = 'UNITED STATES')"
		          + "        or (n1.n_name = 'UNITED STATES' and n2.n_name = 'EGYPT')"
		          + "      )"
		          + "  ) "
		          + "select"
		          + "  supp_nation, cust_nation, l_year, sum(volume) as revenue"
		          + "from shipping"
		          + "group by"
		          + "  supp_nation,"
		          + "  cust_nation,"
		          + "  l_year"
		          + "order by"
		          + "  supp_nation,"
		          + "  cust_nation,"
		          + "  l_year",
				          
		          // 08 rewrite
		          "WITH order_year AS ("
		          + "     SELECT o.o_orderkey, o.o_custkey, extract(year from o.o_orderdate) as o_year"
		          + "     FROM orders o"
		          + "     WHERE  o.o_orderdate >= date '1995-01-01'"
		          + "       AND  o.o_orderdate <= date '1996-12-31'"
		          + "),"
		          + "supplier_nation AS ("
		          + "		SELECT n_nationkey, n_name, n_name = 'EGYPT' nation_check"
		          + "		FROM nation),"
		          + "all_nations AS ("
		          + "    select"
		          + "      o_year,"
		          + "      l.l_extendedprice * (1 - l.l_discount) as volume,"
		          + "      n2.n_name as nation,"
		          + "      nation_check"
		          + "    from"
		          + "      part p,"
		          + "      supplier s,"
		          + "      lineitem l,"
		          + "      order_year o,"
		          + "      customer c,"
		          + "      nation n1,"
		          + "      supplier_nation n2,"
		          + "      region r"
		          + "    where"
		          + "      p.p_partkey = l.l_partkey"
		          + "      and s.s_suppkey = l.l_suppkey"
		          + "      and l.l_orderkey = o.o_orderkey"
		          + "      and o.o_custkey = c.c_custkey"
		          + "      and c.c_nationkey = n1.n_nationkey"
		          + "      and n1.n_regionkey = r.r_regionkey"
		          + "      and r.r_name = 'MIDDLE EAST'"
		          + "      and s.s_nationkey = n2.n_nationkey"
		          + "      and p.p_type = 'PROMO BRUSHED COPPER')"
		          + "select"
		          + "  o_year,"
		          + "  sum(case"
		          + "    when nation_check then volume"
		          + "    else 0"
		          + "  end) / sum(volume) as mkt_share"
		          + "from all_nations"
		          + "group by"
		          + "  o_year"
		          + "order by"
		          + "  o_year",

		          // 09
		          "WITH order_years AS ("
		          + "     SELECT extract(year from o_orderdate) as o_year, o.o_orderkey"
		          + "     FROM orders o),"
		          + "     yellow_parts AS ("
		          + "         SELECT p_partkey"
		          + "	 FROM part"
		          + "	 WHERE p_name like '%yellow%'),"
		          + "     profit AS ("
		          + "         select"
		          + "    	   n_name as nation,"
		          + "     	    o_year,"
		          + "      	    l.l_extendedprice * (1 - l.l_discount) - ps.ps_supplycost * l.l_quantity as amount"
		          + "    	  from"
		          + "    	    yellow_parts p,"
		          + "      	    supplier s,"
		          + "      	    lineitem l,"
		          + "      	    partsupp ps,"
		          + "      	    order_years o,"
		          + "      	    nation n"
		          + "    where"
		          + "      s.s_suppkey = l.l_suppkey"
		          + "      and ps.ps_suppkey = l.l_suppkey"
		          + "      and ps.ps_partkey = l.l_partkey"
		          + "      and p.p_partkey = l.l_partkey"
		          + "      and o.o_orderkey = l.l_orderkey"
		          + "      and s.s_nationkey = n.n_nationkey)"
		          + "select"
		          + "  nation,"
		          + "  o_year,"
		          + "  sum(amount) as sum_profit"
		          + "from profit"
		          + "group by"
		          + "  nation,"
		          + "  o_year"
		          + "order by"
		          + "  nation,"
		          + "  o_year desc",

		      // 10
		      "select"
		          + "  c.c_custkey,"
		          + "  c.c_name,"
		          + "  sum(l.l_extendedprice * (1 - l.l_discount)) as revenue,"
		          + "  c.c_acctbal,"
		          + "  n.n_name,"
		          + "  c.c_address,"
		          + "  c.c_phone,"
		          + "  c.c_comment"
		          + "from"
		          + "  customer c,"
		          + "  orders o,"
		          + "  lineitem l,"
		          + "  nation n"
		          + "where"
		          + "  c.c_custkey = o.o_custkey"
		          + "  and l.l_orderkey = o.o_orderkey"
		          + "  and o.o_orderdate >= date '1994-03-01'"
		          + "  and o.o_orderdate < date '1994-06-01'"
		          + "  and l.l_returnflag = 'R'"
		          + "  and c.c_nationkey = n.n_nationkey"
		          + "group by"
		          + "  c.c_custkey,"
		          + "  c.c_name,"
		          + "  c.c_acctbal,"
		          + "  c.c_phone,"
		          + "  n.n_name,"
		          + "  c.c_address,"
		          + "  c.c_comment"
		          + "order by"
		          + "  revenue desc"
		          + "limit 20",

		      // 11
		          "WITH percentile AS ("
		          + "     SELECT SUM(psp.ps_supplycost * psp.ps_availqty) * 0.0001000000"
		          + "     FROM"
		          + "        partsupp psp,"
		          + "        supplier sp,"
		          + "        nation np"
		          + "      where"
		          + "        psp.ps_suppkey = sp.s_suppkey"
		          + "        and sp.s_nationkey = np.n_nationkey"
		          + "        and np.n_name = 'JAPAN'"
		          + "    )"
		          + "SELECT"
		          + "  ps.ps_partkey,"
		          + "  sum(ps.ps_supplycost * ps.ps_availqty) as val"
		          + "FROM"
		          + "  partsupp ps,"
		          + "  supplier s,"
		          + "  nation n"
		          + "WHERE"
		          + "  ps.ps_suppkey = s.s_suppkey"
		          + "  and s.s_nationkey = n.n_nationkey"
		          + "  and n.n_name = 'JAPAN'"
		          + "GROUP BY ps.ps_partkey "
		          + "HAVING sum(ps.ps_supplycost * ps.ps_availqty) > (SELECT * FROM percentile)"
		          + "ORDER BY val DESC", 

		      // 12
		      "select"
		          + "  l.l_shipmode,"
		          + "  sum(case"
		          + "    when o.o_orderpriority = '1-URGENT'"
		          + "      or o.o_orderpriority = '2-HIGH'"
		          + "      then 1"
		          + "    else 0"
		          + "  end) as high_line_count,"
		          + "  sum(case"
		          + "    when o.o_orderpriority <> '1-URGENT'"
		          + "      and o.o_orderpriority <> '2-HIGH'"
		          + "      then 1"
		          + "    else 0"
		          + "  end) as low_line_count"
		          + "from"
		          + "  orders o,"
		          + "  lineitem l"
		          + "where"
		          + "  o.o_orderkey = l.l_orderkey"
		          + "  and l.l_shipmode in ('TRUCK', 'REG AIR')"
		          + "  and l.l_commitdate < l.l_receiptdate"
		          + "  and l.l_shipdate < l.l_commitdate"
		          + "  and l.l_receiptdate >= date '1994-01-01'"
		          + "  and l.l_receiptdate < date '1995-01-01'"
		          + "group by"
		          + "  l.l_shipmode"
		          + "order by"
		          + "  l.l_shipmode",



		      // 13
		          "WITH c_orders AS ("
		          + "     SELECT c.c_custkey, COUNT(o.o_orderkey) c_count   "
		          + "     FROM   customer c  LEFT OUTER JOIN orders o ON c.c_custkey = o.o_custkey  AND o.o_comment NOT LIKE '%special%requests%'   "
		          + "     GROUP BY   c.c_custkey )"
		          + "SELECT  c_count,   count(*) as custdist "
		          + "FROM    c_orders"
		          + "GROUP BY c_count "
		          + "ORDER BY custdist DESC, c_count DESC",		          

		
			// 14
		      "select"
		          + "  100.00 * sum(case"
		          + "    when p.p_type like 'PROMO%'"
		          + "      then l.l_extendedprice * (1 - l.l_discount)"
		          + "    else 0"
		          + "  end) / sum(l.l_extendedprice * (1 - l.l_discount)) as promo_revenue"
		          + "from"
		          + "  lineitem l,"
		          + "  part p"
		          + "where"
		          + "  l.l_partkey = p.p_partkey"
		          + "  and l.l_shipdate >= date '1994-08-01'"
		          + "  and l.l_shipdate < date '1994-09-01'",

		      // 15
		      "with revenue0 (supplier_no, total_revenue) as ("
		          + "  select"
		          + "    l_suppkey,"
		          + "    sum(l_extendedprice * (1 - l_discount))"
		          + "  from"
		          + "    lineitem"
		          + "  where"
		          + "    l_shipdate >= date '1993-05-01'"
		          + "    and l_shipdate < date '1993-08-01'"
		          + "  group by"
		          + "    l_suppkey)"
		          + "select"
		          + "  s.s_suppkey,"
		          + "  s.s_name,"
		          + "  s.s_address,"
		          + "  s.s_phone,"
		          + "  r.total_revenue"
		          + "from"
		          + "  supplier s,"
		          + "  revenue0 r"
		          + "where"
		          + "  s.s_suppkey = r.supplier_no"
		          + "  and r.total_revenue = ("
		          + "    select"
		          + "      max(total_revenue)"
		          + "    from"
		          + "      revenue0"
		          + "  )"
		          + "order by"
		          + "  s.s_suppkey",

		          // 16
		          // breaking this into smaller pieces for easier debugging
		          "WITH complaints AS (SELECT * FROM supplier WHERE s_comment LIKE '%Customer%Complaints%')," +
						  "     ps_suppkey_set_diff AS (" +
						  "     SELECT ps_partkey, ps_suppkey" +
						  "     FROM partsupp ps LEFT JOIN complaints c ON ps.ps_suppkey = c.s_suppkey" +
						  "     WHERE c.s_suppkey IS NULL)," +
						  "" +
						  "     part_pro  AS ( SELECT p_partkey, p_brand,p_type,p_size " + 
						  "   FROM part p " +
						  "      WHERE p.p_brand <> 'Brand#21'" +
						  "      AND p.p_type not like 'MEDIUM PLATED%'" +
						  "        AND p.p_size IN (38, 2, 8, 31, 44, 5, 14, 24)" +
						  "      )" +
						  "" +
						  "SELECT" +
						  "   p.p_brand," +
						  "   p.p_type," +
						  "   p.p_size," +
						  "   COUNT(DISTINCT ps.ps_suppkey) as supplier_cnt" +
						  " FROM" +
						  "   ps_suppkey_set_diff ps," +
						  "   part_pro p" +
						  " WHERE" +
						  "   p.p_partkey = ps.ps_partkey" +
						  "   " +
						  " GROUP BY" +
						  "   p.p_brand," +
						  "   p.p_type," +
						  "   p.p_size" +
						  " ORDER BY" +
						  "   supplier_cnt desc," +
						  "   p.p_brand," +
						  "   p.p_type," +
						  "   p.p_size",

		                  
		      // 17
		                  
		                  "SELECT sum(extendedprice) / 7.0 as avg_yearly "
		                  + "FROM ( "
		                  + "     SELECT l_quantity as quantity, l_extendedprice as extendedprice, t_avg_quantity"
		                  + "     FROM ("
		                  + "       SELECT l_partkey as t_partkey, 0.2 * avg(l_quantity) as t_avg_quantity "
		                  + "       FROM lineitem"
		                  + "       GROUP By l_partkey) as tmp"
		                  + "    INNER JOIN ("
		                  + "      SELECT l_quantity, l_partkey, l_extendedprice"
		                  + "      FROM  part, lineitem "
		                  + "      WHERE  p_partkey = l_partkey and p_brand = 'Brand#23' and p_container = 'MED BOX'"
		                  + "      ) as l1 ON  l1.l_partkey = t_partkey ) a"
		                  + "where quantity < t_avg_quantity",

			// 18
		      "select"
		          + "  c.c_name,"
		          + "  c.c_custkey,"
		          + "  o.o_orderkey,"
		          + "  o.o_orderdate,"
		          + "  o.o_totalprice,"
		          + "  sum(l.l_quantity)"
		          + "from"
		          + "  customer c,"
		          + "  orders o,"
		          + "  lineitem l"
		          + "where"
		          + "  o.o_orderkey in ("
		          + "    select"
		          + "      l_orderkey"
		          + "    from"
		          + "      lineitem"
		          + "    group by"
		          + "      l_orderkey having"
		          + "        sum(l_quantity) > 313"
		          + "  )"
		          + "  and c.c_custkey = o.o_custkey"
		          + "  and o.o_orderkey = l.l_orderkey"
		          + "group by"
		          + "  c.c_name,"
		          + "  c.c_custkey,"
		          + "  o.o_orderkey,"
		          + "  o.o_orderdate,"
		          + "  o.o_totalprice"
		          + "order by"
		          + "  o.o_totalprice desc,"
		          + "  o.o_orderdate"
		          + "limit 100",

		      // 19
		      "select"
		          + "  sum(l.l_extendedprice* (1 - l.l_discount)) as revenue"
		          + "from"
		          + "  lineitem l,"
		          + "  part p"
		          + "where"
		          + "  ("
		          + "    p.p_partkey = l.l_partkey"
		          + "    and p.p_brand = 'Brand#41'"
		          + "    and p.p_container in ('SM CASE', 'SM BOX', 'SM PACK', 'SM PKG')"
		          + "    and l.l_quantity >= 2 and l.l_quantity <= 2 + 10"
		          + "    and p.p_size between 1 and 5"
		          + "    and l.l_shipmode in ('AIR', 'AIR REG')"
		          + "    and l.l_shipinstruct = 'DELIVER IN PERSON'"
		          + "  )"
		          + "  or"
		          + "  ("
		          + "    p.p_partkey = l.l_partkey"
		          + "    and p.p_brand = 'Brand#13'"
		          + "    and p.p_container in ('MED BAG', 'MED BOX', 'MED PKG', 'MED PACK')"
		          + "    and l.l_quantity >= 14 and l.l_quantity <= 14 + 10"
		          + "    and p.p_size between 1 and 10"
		          + "    and l.l_shipmode in ('AIR', 'AIR REG')"
		          + "    and l.l_shipinstruct = 'DELIVER IN PERSON'"
		          + "  )"
		          + "  or"
		          + "  ("
		          + "    p.p_partkey = l.l_partkey"
		          + "    and p.p_brand = 'Brand#55'"
		          + "    and p.p_container in ('LG CASE', 'LG BOX', 'LG PACK', 'LG PKG')"
		          + "    and l.l_quantity >= 23 and l.l_quantity <= 23 + 10"
		          + "    and p.p_size between 1 and 15"
		          + "    and l.l_shipmode in ('AIR', 'AIR REG')"
		          + "    and l.l_shipinstruct = 'DELIVER IN PERSON'"
		          + "  )",

		      // 20
		          "WITH antiques AS (SELECT p.p_partkey"
		          + "                                 from"
		          + "                                   part p"
		          + "                                 where"
		          + "                                   p.p_name like 'antique%'"
		          + "                                ),"
		          + "            lineitem_agg AS ("
		          + "                            select"
		          + "                                   l_partkey, l_suppkey, 0.5 * sum(l.l_quantity) inventory"
		          + "                                 from"
		          + "                                   lineitem l"
		          + "                                 where"
		          + "                                    l.l_shipdate >= date '1993-01-01'"
		          + "                                   and l.l_shipdate < date '1994-01-01'"
		          + "                              GROUP BY l_partkey, l_suppkey"
		          + "                     ),"
		          + ""
		          + "            ps_qualified AS ("
		          + "	        SELECT ps_suppkey, ps_partkey, ps.ps_availqty"
		          + "                FROM  partsupp ps LEFT JOIN antiques a ON ps.ps_partkey = a.p_partkey"
		          + "                 WHERE  a.p_partkey IS NOT NULL),"
		          + "            qualified AS ("
		          + "                          SELECT DISTINCT ps.ps_suppkey"
		          + "                          FROM lineitem_agg la INNER JOIN ps_qualified ps ON la.l_partkey = ps.ps_partkey AND la.l_suppkey = ps.ps_suppkey"
		          + "                           WHERE ps.ps_availqty > la.inventory"
		          + "                            )"
		          + "select"
		          + "                        s.s_name,"
		          + "                        s.s_address"
		          + "                      from"
		          + "                        supplier s LEFT JOIN qualified q ON s.s_suppkey = q.ps_suppkey,"
		          + "                        nation n"
		          + "                      where"
		          + "                        ps_suppkey IS NOT NULL"
		          + "                        and s.s_nationkey = n.n_nationkey"
		          + "                        and n.n_name = 'KENYA'"
		          + "                      order by"
		          + "                        s.s_name",

		          
					//21
					" WITH l2 AS ( select DISTINCT l_suppkey, l_orderkey"
					+ "                                              from"
					+ "                                                lineitem l1),"
					+ ""
					+ ""
					+ " l1prime AS (SELECT DISTINCT l1.l_suppkey, l1.l_orderkey "
					+ "   FROM lineitem l1 LEFT JOIN l2 ON (l2.l_orderkey = l1.l_orderkey"
					+ " AND l2.l_suppkey <> l1.l_suppkey)"
					+ "                                       WHERE l2.l_orderkey IS NOT NULL),"
					+ ""
					+ ""
					+ "  l3 AS ("
					+ "       select DISTINCT  l_orderkey, l_suppkey"
					+ "      from lineitem l3"
					+ "      where l3.l_receiptdate > l3.l_commitdate"
					+ "   ),"
					+ ""
					+ "  l1doubleprime AS (SELECT l1prime.l_suppkey, l1prime.l_orderkey, l1prime.l_receiptdate, l1prime.l_commitdate"
					+ "     FROM lineitem l1prime LEFT JOIN l3 ON l3.l_orderkey = l1prime.l_orderkey  and l3.l_suppkey <> l1prime.l_suppkey"
					+ "     WHERE  l3.l_orderkey IS NULL),"
					+ ""
					+ " agg AS (select"
					+ "    s.s_name, l_orderkey, l_suppkey"
					+ "  from"
					+ "    supplier s,"
					+ "    l1doubleprime l1,"
					+ "    orders o,"
					+ "    nation n"
					+ "  where"
					+ "    s.s_suppkey = l1.l_suppkey"
					+ "    and o.o_orderkey = l1.l_orderkey"
					+ "    and o.o_orderstatus = 'F'"
					+ "    and l1.l_receiptdate > l1.l_commitdate"
					+ "    and s.s_nationkey = n.n_nationkey"
					+ "    and n.n_name = 'BRAZIL'"
					+ "  order by"
					+ "    s.s_name)"
					+ ""
					+ " SELECT agg.s_name, count(*) as numwait FROM agg LEFT JOIN l1prime on agg.l_orderkey = l1prime.l_orderkey and agg.l_suppkey = l1prime.l_suppkey WHERE l1prime.l_suppkey IS NOT NULL"
					+ " group by"
					+ "   agg.s_name"
					+ " order by"
					+ "   numwait desc,"
					+ "   agg.s_name"
					+ " limit 100",




				      // 22
					 "WITH customer_selection AS ("
					 + "       SELECT substring(c_phone from 1 for 2) as cntrycode, c_acctbal, c_custkey"
					 + "       FROM customer"
					 + "       WHERE substring(c_phone from 1 for 2) in"
					 + "            ('24', '31', '11', '16', '21', '20', '34')),"
					 + "    c_avg_acctbal AS ("
					 + "	      SELECT avg(c_acctbal) "
					 + "	      FROM customer_selection"
					 + "	      WHERE c_acctbal > 0.0)"
					 + "SELECT cntrycode, count(*) as numcust, sum(c_acctbal) as totacctbal"
					 + "FROM customer_selection c LEFT OUTER JOIN orders o  ON o.o_custkey = c.c_custkey"
					 + "WHERE c_acctbal > (SELECT * FROM c_avg_acctbal) AND o.o_custkey IS NULL"
					 + "GROUP BY cntrycode"
					 + "ORDER BY cntrycode"						
					
			);



    
    protected static final Map<String, String> TABLE_DEFINITIONS = ImmutableMap.of( 
                    "customer", "CREATE TABLE customer (" + 
            "    c_custkey integer NOT NULL, " + 
            "    c_name character varying(25), " + 
            "    c_address character varying(40), " + 
            "    c_nationkey integer, " + 
            "    c_phone character(15), " + 
            "    c_acctbal numeric, " + 
            "    c_mktsegment character(10), " + 
            "    c_comment character varying(117), " + 
            "    CONSTRAINT c_acctbal_domain CHECK (((c_acctbal >= CAST('-999.99' AS numeric)) AND (c_acctbal <= 9999.99))), " + 
            "    CONSTRAINT c_custkey_domain CHECK (((c_custkey >= 1) AND (c_custkey <= 15000))), " + 
            "    CONSTRAINT c_mktsegment_domain CHECK (c_mktsegment IN ('MACHINERY', 'AUTOMOBILE', 'BUILDING', 'FURNITURE', 'HOUSEHOLD')), " + 
            "    CONSTRAINT c_nationkey_domain CHECK (((c_nationkey >= 0) AND (c_nationkey <= 24)))" + 
            ")",
            "lineitem", 
            "CREATE TABLE lineitem (" + 
            "    l_orderkey integer NOT NULL," + 
            "    l_partkey integer," + 
            "    l_suppkey integer," + 
            "    l_linenumber integer NOT NULL," + 
            "    l_quantity numeric," + 
            "    l_extendedprice numeric," + 
            "    l_discount numeric," + 
            "    l_tax numeric," + 
            "    l_returnflag character(1)," + 
            "    l_linestatus character(1)," + 
            "    l_shipdate date," + 
            "    l_commitdate date," + 
            "    l_receiptdate date," + 
            "    l_shipinstruct character(25)," + 
            "    l_shipmode character(10)," + 
            "    l_comment character varying(44)," + 
            "    CONSTRAINT l_discount_domain CHECK (((l_discount >= 0.00) AND (l_discount <= 0.10)))," + 
            "    CONSTRAINT l_linenumber_domain CHECK (((l_linenumber >= 1) AND (l_linenumber <= 7)))," + 
            "    CONSTRAINT l_linestatus_based_on_shipdate_domain CHECK ((((l_linestatus = 'O')) AND (l_shipdate > CAST('1995-06-17' AS date))) OR (l_linestatus = 'F' AND (l_shipdate <= CAST('1995-06-17' AS date))))," + 
            "    CONSTRAINT l_linestatus_domain CHECK (l_linestatus IN ('O', 'F'))," + 
            "    CONSTRAINT l_partkey_domain CHECK ((l_partkey >= 1) AND (l_partkey <= 20000))," + 
            "    CONSTRAINT l_quantity_domain CHECK ((l_quantity >= 1) AND (l_quantity <= 50))," + 
            "    CONSTRAINT l_receiptdate_domain CHECK ((l_receiptdate >= (l_shipdate + 1)) AND (l_receiptdate <= (l_shipdate + 30)))," + 
            "    CONSTRAINT l_returnflag_based_on_receiptdate_domain CHECK ((l_returnflag IN ('R', 'A') AND l_receiptdate <= CAST('1995-06-17' AS date)) OR (l_returnflag = 'N' AND l_receiptdate > CAST('1995-06-17' AS date)))," + 
            "    CONSTRAINT l_returnflag_domain CHECK (l_returnflag IN ('R', 'A', 'N'))," + 
            "    CONSTRAINT l_shipinstruct_domain CHECK (l_shipinstruct IN ('DELIVER IN PERSON', 'COLLECT COD', 'NONE','TAKE BACK RETURN'))," + 
            "    CONSTRAINT l_shipmode_domain CHECK (l_shipmode IN ('REG AIR', 'AIR', 'RAIL', 'SHIP', 'TRUCK', 'MAIL', 'FOB'))," + 
            "    CONSTRAINT l_tax_domain CHECK (l_tax >= 0.00 AND l_tax <= 0.08)" + 
            ")");


    
	  protected void setUp() throws Exception {
		  SystemConfiguration.getInstance().closeCalciteConnection();
		  SystemConfiguration.resetConfiguration();
		  SystemCatalog.resetInstance();
		  ConnectionManager.reset();
		  
		  config = SystemConfiguration.getInstance("tpch");
		  catalog = SystemCatalog.getInstance();

		  parser = new SqlStatementParser();
		  honestBroker = SystemConfiguration.getInstance().getHonestBrokerConfig();
		  logger = SystemConfiguration.getInstance().getLogger();
		  dialect = config.DIALECT;
		 

	  }

	  
	  protected String readSQL(int queryNo) {
		  return QUERIES.get(queryNo-1); // zero-indexed
	  }
	  

	  @Override
	  protected void tearDown() throws Exception {

		  // clean up any dangling resources
		  ConnectionManager connections = ConnectionManager.getInstance();
		  if(connections != null) {
			  connections.closeConnections();
			  ConnectionManager.reset();
		  }
		  
		  SystemConfiguration.getInstance().closeCalciteConnection();
		  SystemConfiguration.resetConfiguration();
		  SystemCatalog.resetInstance();
		  
		  // delete any generated classfiles
		  String classFiles = Utilities.getVaultDBRoot() + "/target/classes/org/vaultdb/compiler/emp/generated/*.class";
		  Utilities.runCmd("rm " + classFiles);

		  
		  
		  

		  
	  }


	public QueryTable getExpectedOutput(String query, SecureRelRecordType outSchema) throws Exception {
	    String unionedId = ConnectionManager.getInstance().getUnioned();
	    return SqlQueryExecutor.query(query, outSchema, unionedId);
	    }





}
