package org.smcql.planner;

import java.util.List;
import java.util.logging.Level;

import org.apache.calcite.plan.RelOptUtil;
import org.smcql.parser.tpch.TpcHBaseTest;
import org.smcql.plan.SecureRelRoot;

import com.google.common.collect.ImmutableList;


// basically need a broader model for deciding whether to execute in the clear or obliviously:
// * sensitivity of attributes we compute on
// * partitioning of data - partitioned-alike = plaintext exec w/obliv padding
// * replication status of tables
// * exec mode of max child (e.g., run filter in the clear on private data w/obliv padding, subsequent distributed ops are oblivious + MPC)

// this is the quad chart from before  (local, distributed (MPC) ) x (oblivious, cleartext)


// need to push down joins that can be executed in the clear, either owing to permissions, replication, or partitioning

// query rewrite rules (in order of operations):
// 1) push down joins with replicated tables
// 2) push down joins on public attributes (run after joins in #1)
// 3) push down joins that are partitioned-alike (run after joins in #2)

public class TreeBuilderTpcHTest extends TpcHBaseTest {

	  protected void setUp() throws Exception {
		    
		 super.setUp();
			
	  }

		protected static final List<String> PLANS = ImmutableList.of(
				// 01
				"LogicalSort-DistributedOblivious, schema:(#0: l_returnflag CHAR(1) Private,#1: l_linestatus CHAR(1) Private,#2: sum_qty DECIMAL(19, 0) Private,#3: sum_base_price DECIMAL(19, 0) Private,#4: sum_disc_price DECIMAL(19, 0) Private,#5: sum_charge DECIMAL(19, 0) Private,#6: avg_qty DECIMAL(19, 0) Private,#7: avg_price DECIMAL(19, 0) Private,#8: avg_disc DECIMAL(19, 0) Private,#9: count_order BIGINT Private)\n"
				 + "    LogicalAggregate-DistributedOblivious, schema:(#0: l_returnflag CHAR(1) Private,#1: l_linestatus CHAR(1) Private,#2: sum_qty DECIMAL(19, 0) Private,#3: sum_base_price DECIMAL(19, 0) Private,#4: sum_disc_price DECIMAL(19, 0) Private,#5: sum_charge DECIMAL(19, 0) Private,#6: avg_qty DECIMAL(19, 0) Private,#7: avg_price DECIMAL(19, 0) Private,#8: avg_disc DECIMAL(19, 0) Private,#9: count_order BIGINT Private)\n"
				 + "        LogicalProject-LocalObliviousPartitioned, schema:(#0: l_returnflag CHAR(1) Private,#1: l_linestatus CHAR(1) Private,#2: l_quantity DECIMAL(19, 0) Private,#3: l_extendedprice DECIMAL(19, 0) Private,#4: $f4 DECIMAL(19, 0) Private,#5: $f5 DECIMAL(19, 0) Private,#6: l_discount DECIMAL(19, 0) Private)\n"
				 + "            LogicalFilter-LocalObliviousPartitioned, schema:(#0: l_quantity DECIMAL(19, 0) Private,#1: l_extendedprice DECIMAL(19, 0) Private,#2: l_discount DECIMAL(19, 0) Private,#3: l_tax DECIMAL(19, 0) Private,#4: l_returnflag CHAR(1) Private,#5: l_linestatus CHAR(1) Private,#6: l_shipdate DATE Private)\n"
				 + "                LogicalProject-LocalClearPartitioned, schema:(#0: l_quantity DECIMAL(19, 0) Private,#1: l_extendedprice DECIMAL(19, 0) Private,#2: l_discount DECIMAL(19, 0) Private,#3: l_tax DECIMAL(19, 0) Private,#4: l_returnflag CHAR(1) Private,#5: l_linestatus CHAR(1) Private,#6: l_shipdate DATE Private)\n"
				 + "                    JdbcTableScan-LocalClearPartitioned, schema:(#0: l_orderkey INTEGER Private,#1: l_partkey INTEGER Private,#2: l_suppkey INTEGER Private,#3: l_linenumber INTEGER Private,#4: l_quantity DECIMAL(19, 0) Private,#5: l_extendedprice DECIMAL(19, 0) Private,#6: l_discount DECIMAL(19, 0) Private,#7: l_tax DECIMAL(19, 0) Private,#8: l_returnflag CHAR(1) Private,#9: l_linestatus CHAR(1) Private,#10: l_shipdate DATE Private,#11: l_commitdate DATE Private,#12: l_receiptdate DATE Private,#13: l_shipinstruct CHAR(25) Private,#14: l_shipmode CHAR(10) Private,#15: l_comment VARCHAR(44) Private)\n",
  
      		  // 02
      		  // all joins on public keys that are partitioned-alike
				 "LogicalProject-DistributedObliviousSharded, schema:(#0: s_acctbal DECIMAL(19, 0) Private,#1: s_name CHAR(25) Public,#2: n_name CHAR(25) Public,#3: p_partkey INTEGER Public,#4: p_mfgr CHAR(25) Public,#5: s_address VARCHAR(40) Private,#6: s_phone CHAR(15) Private,#7: s_comment VARCHAR(101) Private)\n"
				 + "    LogicalSort-DistributedObliviousSharded, schema:(#0: p_partkey INTEGER Public,#1: p_mfgr CHAR(25) Public,#2: s_suppkey INTEGER Public,#3: s_name CHAR(25) Public,#4: s_address VARCHAR(40) Private,#5: s_nationkey BIGINT Public,#6: s_phone CHAR(15) Private,#7: s_acctbal DECIMAL(19, 0) Private,#8: s_comment VARCHAR(101) Private,#9: ps_partkey BIGINT Public,#10: ps_suppkey BIGINT Public,#11: ps_supplycost DECIMAL(19, 0) Public,#12: n_nationkey INTEGER Public,#13: n_name CHAR(25) Public,#14: n_regionkey BIGINT Public,#15: r_regionkey INTEGER Public,#16: ps_partkey0 BIGINT Public,#17: min_cost DECIMAL(19, 0) Public)\n"
				 + "        LogicalJoin-LocalClearSharded, schema:(#0: p_partkey INTEGER Public,#1: p_mfgr CHAR(25) Public,#2: s_suppkey INTEGER Public,#3: s_name CHAR(25) Public,#4: s_address VARCHAR(40) Private,#5: s_nationkey BIGINT Public,#6: s_phone CHAR(15) Private,#7: s_acctbal DECIMAL(19, 0) Private,#8: s_comment VARCHAR(101) Private,#9: ps_partkey BIGINT Public,#10: ps_suppkey BIGINT Public,#11: ps_supplycost DECIMAL(19, 0) Public,#12: n_nationkey INTEGER Public,#13: n_name CHAR(25) Public,#14: n_regionkey BIGINT Public,#15: r_regionkey INTEGER Public,#16: ps_partkey0 BIGINT Public,#17: min_cost DECIMAL(19, 0) Public), Predicate: AND(=($16, $9), =($11, $17)), slice key: [#16: ps_partkey0 BIGINT Public, #9: ps_partkey BIGINT Public]\n"
				 + "            LogicalJoin-LocalClearSharded, schema:(#0: p_partkey INTEGER Public,#1: p_mfgr CHAR(25) Public,#2: s_suppkey INTEGER Public,#3: s_name CHAR(25) Public,#4: s_address VARCHAR(40) Private,#5: s_nationkey BIGINT Public,#6: s_phone CHAR(15) Private,#7: s_acctbal DECIMAL(19, 0) Private,#8: s_comment VARCHAR(101) Private,#9: ps_partkey BIGINT Public,#10: ps_suppkey BIGINT Public,#11: ps_supplycost DECIMAL(19, 0) Public,#12: n_nationkey INTEGER Public,#13: n_name CHAR(25) Public,#14: n_regionkey BIGINT Public,#15: r_regionkey INTEGER Public), Predicate: =($14, CAST($15):BIGINT NOT NULL)\n"
				 + "                LogicalJoin-LocalClearSharded, schema:(#0: p_partkey INTEGER Public,#1: p_mfgr CHAR(25) Public,#2: s_suppkey INTEGER Public,#3: s_name CHAR(25) Public,#4: s_address VARCHAR(40) Private,#5: s_nationkey BIGINT Public,#6: s_phone CHAR(15) Private,#7: s_acctbal DECIMAL(19, 0) Private,#8: s_comment VARCHAR(101) Private,#9: ps_partkey BIGINT Public,#10: ps_suppkey BIGINT Public,#11: ps_supplycost DECIMAL(19, 0) Public,#12: n_nationkey INTEGER Public,#13: n_name CHAR(25) Public,#14: n_regionkey BIGINT Public), Predicate: =($5, CAST($12):BIGINT NOT NULL)\n"
				 + "                    LogicalJoin-LocalClearSharded, schema:(#0: p_partkey INTEGER Public,#1: p_mfgr CHAR(25) Public,#2: s_suppkey INTEGER Public,#3: s_name CHAR(25) Public,#4: s_address VARCHAR(40) Private,#5: s_nationkey BIGINT Public,#6: s_phone CHAR(15) Private,#7: s_acctbal DECIMAL(19, 0) Private,#8: s_comment VARCHAR(101) Private,#9: ps_partkey BIGINT Public,#10: ps_suppkey BIGINT Public,#11: ps_supplycost DECIMAL(19, 0) Public), Predicate: AND(=(CAST($0):BIGINT NOT NULL, $9), =(CAST($2):BIGINT NOT NULL, $10))\n"
				 + "                        LogicalJoin-LocalClearSharded, schema:(#0: p_partkey INTEGER Public,#1: p_mfgr CHAR(25) Public,#2: s_suppkey INTEGER Public,#3: s_name CHAR(25) Public,#4: s_address VARCHAR(40) Private,#5: s_nationkey BIGINT Public,#6: s_phone CHAR(15) Private,#7: s_acctbal DECIMAL(19, 0) Private,#8: s_comment VARCHAR(101) Private), Predicate: true\n"
				 + "                            LogicalProject-LocalClearReplicated, schema:(#0: p_partkey INTEGER Public,#1: p_mfgr CHAR(25) Public)\n"
				 + "                                LogicalFilter-LocalClearReplicated, schema:(#0: p_partkey INTEGER Public,#1: p_mfgr CHAR(25) Public,#2: p_type VARCHAR(25) Public,#3: p_size INTEGER Public)\n"
				 + "                                    LogicalProject-LocalClearReplicated, schema:(#0: p_partkey INTEGER Public,#1: p_mfgr CHAR(25) Public,#2: p_type VARCHAR(25) Public,#3: p_size INTEGER Public)\n"
				 + "                                        JdbcTableScan-LocalClearReplicated, schema:(#0: p_partkey INTEGER Public,#1: p_name VARCHAR(55) Public,#2: p_mfgr CHAR(25) Public,#3: p_brand CHAR(10) Public,#4: p_type VARCHAR(25) Public,#5: p_size INTEGER Public,#6: p_container CHAR(10) Public,#7: p_retailprice DECIMAL(19, 0) Public,#8: p_comment VARCHAR(23) Public)\n"
				 + "                            JdbcTableScan-LocalClearSharded, schema:(#0: s_suppkey INTEGER Public,#1: s_name CHAR(25) Public,#2: s_address VARCHAR(40) Private,#3: s_nationkey BIGINT Public,#4: s_phone CHAR(15) Private,#5: s_acctbal DECIMAL(19, 0) Private,#6: s_comment VARCHAR(101) Private)\n"
				 + "                        LogicalProject-LocalClearSharded, schema:(#0: ps_partkey BIGINT Public,#1: ps_suppkey BIGINT Public,#2: ps_supplycost DECIMAL(19, 0) Public)\n"
				 + "                            JdbcTableScan-LocalClearSharded, schema:(#0: ps_partkey BIGINT Public,#1: ps_suppkey BIGINT Public,#2: ps_availqty INTEGER Private,#3: ps_supplycost DECIMAL(19, 0) Public,#4: ps_comment VARCHAR(199) Private)\n"
				 + "                    LogicalProject-LocalClearReplicated, schema:(#0: n_nationkey INTEGER Public,#1: n_name CHAR(25) Public,#2: n_regionkey BIGINT Public)\n"
				 + "                        JdbcTableScan-LocalClearReplicate, schema:(#0: n_nationkey INTEGER Public,#1: n_name CHAR(25) Public,#2: n_regionkey BIGINT Public,#3: n_comment VARCHAR(152) Public)\n"
				 + "                LogicalProject-LocalClearReplicated, schema:(#0: r_regionkey INTEGER Public)\n"
				 + "                    LogicalFilter-LocalClearReplicated, schema:(#0: r_regionkey INTEGER Public,#1: r_name CHAR(25) Public)\n"
				 + "                        LogicalProject-LocalClearReplicated, schema:(#0: r_regionkey INTEGER Public,#1: r_name CHAR(25) Public)\n"
				 + "                            JdbcTableScan-LocalClearReplicated, schema:(#0: r_regionkey INTEGER Public,#1: r_name CHAR(25) Public,#2: r_comment VARCHAR(152) Public)\n"
				 + "            LogicalAggregate-DistributedObliviousSharded, schema:(#0: ps_partkey BIGINT Public,#1: min_cost DECIMAL(19, 0) Public), slice key: [#0: ps_partkey BIGINT Public, #1: ps_supplycost DECIMAL(19, 0) Public]\n"
				 + "                LogicalProject-LocalClearSharded, schema:(#0: ps_partkey BIGINT Public,#1: ps_supplycost DECIMAL(19, 0) Public)\n"
				 + "                    LogicalJoin-LocalClearReplicated, schema:(#0: ps_partkey BIGINT Public,#1: ps_suppkey BIGINT Public,#2: ps_supplycost DECIMAL(19, 0) Public,#3: s_suppkey INTEGER Public,#4: s_nationkey BIGINT Public,#5: n_nationkey INTEGER Public,#6: n_regionkey BIGINT Public,#7: r_regionkey INTEGER Public), Predicate: =($6, CAST($7):BIGINT NOT NULL)\n"
				 + "                        LogicalJoin-LocalClearSharded, schema:(#0: ps_partkey BIGINT Public,#1: ps_suppkey BIGINT Public,#2: ps_supplycost DECIMAL(19, 0) Public,#3: s_suppkey INTEGER Public,#4: s_nationkey BIGINT Public,#5: n_nationkey INTEGER Public,#6: n_regionkey BIGINT Public), Predicate: =($4, CAST($5):BIGINT NOT NULL)\n"
				 + "                            LogicalJoin-LocalClearSharded, schema:(#0: ps_partkey BIGINT Public,#1: ps_suppkey BIGINT Public,#2: ps_supplycost DECIMAL(19, 0) Public,#3: s_suppkey INTEGER Public,#4: s_nationkey BIGINT Public), Predicate: =(CAST($3):BIGINT NOT NULL, $1)\n"
				 + "                                LogicalProject-LocalClearSharded, schema:(#0: ps_partkey BIGINT Public,#1: ps_suppkey BIGINT Public,#2: ps_supplycost DECIMAL(19, 0) Public)\n"
				 + "                                    JdbcTableScan-LocalClearSharded, schema:(#0: ps_partkey BIGINT Public,#1: ps_suppkey BIGINT Public,#2: ps_availqty INTEGER Private,#3: ps_supplycost DECIMAL(19, 0) Public,#4: ps_comment VARCHAR(199) Private)\n"
				 + "                                LogicalProject-LocalClearSharded, schema:(#0: s_suppkey INTEGER Public,#1: s_nationkey BIGINT Public)\n"
				 + "                                    JdbcTableScan-LocalClearSharded, schema:(#0: s_suppkey INTEGER Public,#1: s_name CHAR(25) Public,#2: s_address VARCHAR(40) Private,#3: s_nationkey BIGINT Public,#4: s_phone CHAR(15) Private,#5: s_acctbal DECIMAL(19, 0) Private,#6: s_comment VARCHAR(101) Private)\n"
				 + "                            LogicalProject-LocalClearReplicated, schema:(#0: n_nationkey INTEGER Public,#1: n_regionkey BIGINT Public)\n"
				 + "                                JdbcTableScan-LocalClearReplicated, schema:(#0: n_nationkey INTEGER Public,#1: n_name CHAR(25) Public,#2: n_regionkey BIGINT Public,#3: n_comment VARCHAR(152) Public)\n"
				 + "                        LogicalProject-LocalClearReplicated, schema:(#0: r_regionkey INTEGER Public)\n"
				 + "                            LogicalFilter-LocalClearReplicated, schema:(#0: r_regionkey INTEGER Public,#1: r_name CHAR(25) Public)\n"
				 + "                                LogicalProject-LocalClearReplicated, schema:(#0: r_regionkey INTEGER Public,#1: r_name CHAR(25) Public)\n"
				 + "                                    JdbcTableScan-LocalClearReplicated, schema:(#0: r_regionkey INTEGER Public,#1: r_name CHAR(25) Public,#2: r_comment VARCHAR(152) Public)\n",
          		  
          		  // 03
				 "LogicalProject-DistributedOblivious, schema:(#0: l_orderkey INTEGER Private,#1: revenue DECIMAL(19, 0) Private,#2: o_orderdate DATE Private,#3: o_shippriority INTEGER Private)\n"
				 + "    LogicalSort-DistributedOblivious, schema:(#0: l_orderkey INTEGER Private,#1: o_orderdate DATE Private,#2: o_shippriority INTEGER Private,#3: revenue DECIMAL(19, 0) Private)\n"
				 + "        LogicalAggregate-DistributedOblivious, schema:(#0: l_orderkey INTEGER Private,#1: o_orderdate DATE Private,#2: o_shippriority INTEGER Private,#3: revenue DECIMAL(19, 0) Private)\n"
				 + "            LogicalProject-LocalObliviousPartitioned, schema:(#0: l_orderkey INTEGER Private,#1: o_orderdate DATE Private,#2: o_shippriority INTEGER Private,#3: $f3 DECIMAL(19, 0) Private)\n"
				 + "                LogicalJoin-LocalObliviousPartitioned, schema:(#0: c_custkey INTEGER Public,#1: o_orderkey INTEGER Public,#2: o_custkey INTEGER Private,#3: o_orderdate DATE Private,#4: o_shippriority INTEGER Private,#5: l_orderkey INTEGER Private,#6: l_extendedprice DECIMAL(19, 0) Private,#7: l_discount DECIMAL(19, 0) Private), Predicate: =($5, $1)\n"
				 + "                    LogicalJoin-LocalObliviousPartitioned, schema:(#0: c_custkey INTEGER Public,#1: o_orderkey INTEGER Public,#2: o_custkey INTEGER Private,#3: o_orderdate DATE Private,#4: o_shippriority INTEGER Private), Predicate: =($0, $2)\n"
				 + "                        LogicalProject-LocalObliviousPartitioned, schema:(#0: c_custkey INTEGER Public)\n"
				 + "                            LogicalFilter-LocalObliviousPartitioned, schema:(#0: c_custkey INTEGER Public,#1: c_mktsegment CHAR(10) Private)\n"
				 + "                                LogicalProject-LocalClearPartitioned, schema:(#0: c_custkey INTEGER Public,#1: c_mktsegment CHAR(10) Private)\n"
				 + "                                    JdbcTableScan-LocalClearPartitioned, schema:(#0: c_custkey INTEGER Public,#1: c_name VARCHAR(25) Private,#2: c_address VARCHAR(40) Private,#3: c_nationkey INTEGER Public,#4: c_phone CHAR(15) Private,#5: c_acctbal DECIMAL(19, 0) Private,#6: c_mktsegment CHAR(10) Private,#7: c_comment VARCHAR(117) Private)\n"
				 + "                        LogicalFilter-LocalObliviousPartitioned, schema:(#0: o_orderkey INTEGER Public,#1: o_custkey INTEGER Private,#2: o_orderdate DATE Private,#3: o_shippriority INTEGER Private)\n"
				 + "                            LogicalProject-LocalClearPartitioned, schema:(#0: o_orderkey INTEGER Public,#1: o_custkey INTEGER Private,#2: o_orderdate DATE Private,#3: o_shippriority INTEGER Private)\n"
				 + "                                JdbcTableScan-LocalClearPartitioned, schema:(#0: o_orderkey INTEGER Public,#1: o_custkey INTEGER Private,#2: o_orderstatus CHAR(1) Private,#3: o_totalprice DECIMAL(19, 0) Private,#4: o_orderdate DATE Private,#5: o_orderpriority CHAR(15) Private,#6: o_clerk CHAR(15) Private,#7: o_shippriority INTEGER Private,#8: o_comment VARCHAR(79) Private)\n"
				 + "                    LogicalProject-LocalObliviousPartitioned, schema:(#0: l_orderkey INTEGER Private,#1: l_extendedprice DECIMAL(19, 0) Private,#2: l_discount DECIMAL(19, 0) Private)\n"
				 + "                        LogicalFilter-LocalObliviousPartitioned, schema:(#0: l_orderkey INTEGER Private,#1: l_extendedprice DECIMAL(19, 0) Private,#2: l_discount DECIMAL(19, 0) Private,#3: l_shipdate DATE Private)\n"
				 + "                            LogicalProject-LocalClearPartitioned, schema:(#0: l_orderkey INTEGER Private,#1: l_extendedprice DECIMAL(19, 0) Private,#2: l_discount DECIMAL(19, 0) Private,#3: l_shipdate DATE Private)\n"
				 + "                                JdbcTableScan-LocalClearPartitioned, schema:(#0: l_orderkey INTEGER Private,#1: l_partkey INTEGER Private,#2: l_suppkey INTEGER Private,#3: l_linenumber INTEGER Private,#4: l_quantity DECIMAL(19, 0) Private,#5: l_extendedprice DECIMAL(19, 0) Private,#6: l_discount DECIMAL(19, 0) Private,#7: l_tax DECIMAL(19, 0) Private,#8: l_returnflag CHAR(1) Private,#9: l_linestatus CHAR(1) Private,#10: l_shipdate DATE Private,#11: l_commitdate DATE Private,#12: l_receiptdate DATE Private,#13: l_shipinstruct CHAR(25) Private,#14: l_shipmode CHAR(10) Private,#15: l_comment VARCHAR(44) Private)\n",

				 // 04
          				"LogicalSort-DistributedOblivious, schema:(#0: o_orderpriority CHAR(15) Private,#1: order_count BIGINT Private)\n"
          				 + "    LogicalAggregate-DistributedOblivious, schema:(#0: o_orderpriority CHAR(15) Private,#1: order_count BIGINT Private)\n"
          				 + "        LogicalProject-LocalObliviousPartitioned, schema:(#0: o_orderpriority CHAR(15) Private)\n"
          				 + "            LogicalJoin-LocalObliviousPartitioned, schema:(#0: o_orderkey INTEGER Public,#1: o_orderpriority CHAR(15) Private,#2: l_orderkey INTEGER Private), Predicate: =($2, $0)\n"
          				 + "                LogicalProject-LocalObliviousPartitioned, schema:(#0: o_orderkey INTEGER Public,#1: o_orderpriority CHAR(15) Private)\n"
          				 + "                    LogicalFilter-LocalObliviousPartitioned, schema:(#0: o_orderkey INTEGER Public,#1: o_orderdate DATE Private,#2: o_orderpriority CHAR(15) Private)\n"
          				 + "                        LogicalProject-LocalClearPartitioned, schema:(#0: o_orderkey INTEGER Public,#1: o_orderdate DATE Private,#2: o_orderpriority CHAR(15) Private)\n"
          				 + "                            JdbcTableScan-LocalClearPartitioned, schema:(#0: o_orderkey INTEGER Public,#1: o_custkey INTEGER Private,#2: o_orderstatus CHAR(1) Private,#3: o_totalprice DECIMAL(19, 0) Private,#4: o_orderdate DATE Private,#5: o_orderpriority CHAR(15) Private,#6: o_clerk CHAR(15) Private,#7: o_shippriority INTEGER Private,#8: o_comment VARCHAR(79) Private)\n"
          				 + "                LogicalProject-LocalObliviousPartitioned, schema:(#0: l_orderkey INTEGER Private)\n"
          				 + "                    LogicalFilter-LocalObliviousPartitioned, schema:(#0: l_orderkey INTEGER Private,#1: l_commitdate DATE Private,#2: l_receiptdate DATE Private)\n"
          				 + "                        LogicalProject-LocalClearPartitioned, schema:(#0: l_orderkey INTEGER Private,#1: l_commitdate DATE Private,#2: l_receiptdate DATE Private)\n"
          				 + "                            JdbcTableScan-LocalClearPartitioned, schema:(#0: l_orderkey INTEGER Private,#1: l_partkey INTEGER Private,#2: l_suppkey INTEGER Private,#3: l_linenumber INTEGER Private,#4: l_quantity DECIMAL(19, 0) Private,#5: l_extendedprice DECIMAL(19, 0) Private,#6: l_discount DECIMAL(19, 0) Private,#7: l_tax DECIMAL(19, 0) Private,#8: l_returnflag CHAR(1) Private,#9: l_linestatus CHAR(1) Private,#10: l_shipdate DATE Private,#11: l_commitdate DATE Private,#12: l_receiptdate DATE Private,#13: l_shipinstruct CHAR(25) Private,#14: l_shipmode CHAR(10) Private,#15: l_comment VARCHAR(44) Private)\n",
      						 
          					// 05
          				"LogicalSort-DistributedOblivious, schema:(#0: n_name CHAR(25) Public,#1: revenue DECIMAL(19, 0) Private)\n"
          				 + "    LogicalAggregate-DistributedOblivious, schema:(#0: n_name CHAR(25) Public,#1: revenue DECIMAL(19, 0) Private), slice key: [#0: n_name CHAR(25) Public]\n"
          				 + "        LogicalProject-DistributedOblivious, schema:(#0: n_name CHAR(25) Public,#1: $f1 DECIMAL(19, 0) Private)\n"
          				 + "            LogicalJoin-DistributedOblivious, schema:(#0: c_custkey INTEGER Public,#1: c_nationkey INTEGER Public,#2: o_orderkey INTEGER Public,#3: o_custkey INTEGER Private,#4: l_orderkey INTEGER Private,#5: l_suppkey INTEGER Private,#6: l_extendedprice DECIMAL(19, 0) Private,#7: l_discount DECIMAL(19, 0) Private,#8: s_suppkey INTEGER Private,#9: s_nationkey INTEGER Public,#10: n_nationkey INTEGER Public,#11: n_name CHAR(25) Public,#12: n_regionkey INTEGER Public,#13: r_regionkey INTEGER Public), Predicate: =($12, $13), slice key: [#12: n_regionkey INTEGER Public, #13: r_regionkey INTEGER Public]\n"
          				 + "                LogicalJoin-DistributedOblivious, schema:(#0: c_custkey INTEGER Public,#1: c_nationkey INTEGER Public,#2: o_orderkey INTEGER Public,#3: o_custkey INTEGER Private,#4: l_orderkey INTEGER Private,#5: l_suppkey INTEGER Private,#6: l_extendedprice DECIMAL(19, 0) Private,#7: l_discount DECIMAL(19, 0) Private,#8: s_suppkey INTEGER Private,#9: s_nationkey INTEGER Public,#10: n_nationkey INTEGER Public,#11: n_name CHAR(25) Public,#12: n_regionkey INTEGER Public), Predicate: =($9, $10), slice key: [#9: s_nationkey INTEGER Public, #10: n_nationkey INTEGER Public]\n"
          				 + "                    LogicalJoin-DistributedOblivious, schema:(#0: c_custkey INTEGER Public,#1: c_nationkey INTEGER Public,#2: o_orderkey INTEGER Public,#3: o_custkey INTEGER Private,#4: l_orderkey INTEGER Private,#5: l_suppkey INTEGER Private,#6: l_extendedprice DECIMAL(19, 0) Private,#7: l_discount DECIMAL(19, 0) Private,#8: s_suppkey INTEGER Private,#9: s_nationkey INTEGER Public), Predicate: AND(=($5, $8), =($1, $9)), slice key: [#1: c_nationkey INTEGER Public, #9: s_nationkey INTEGER Public]\n"
          				 + "                        LogicalJoin-LocalObliviousPartitioned, schema:(#0: c_custkey INTEGER Public,#1: c_nationkey INTEGER Public,#2: o_orderkey INTEGER Public,#3: o_custkey INTEGER Private,#4: l_orderkey INTEGER Private,#5: l_suppkey INTEGER Private,#6: l_extendedprice DECIMAL(19, 0) Private,#7: l_discount DECIMAL(19, 0) Private), Predicate: =($4, $2)\n"
          				 + "                            LogicalJoin-LocalObliviousPartitioned, schema:(#0: c_custkey INTEGER Public,#1: c_nationkey INTEGER Public,#2: o_orderkey INTEGER Public,#3: o_custkey INTEGER Private), Predicate: =($0, $3)\n"
          				 + "                                LogicalProject-LocalObliviousPartitioned, schema:(#0: c_custkey INTEGER Public,#1: c_nationkey INTEGER Public)\n"
          				 + "                                    JdbcTableScan-LocalClearPartitioned, schema:(#0: c_custkey INTEGER Public,#1: c_name VARCHAR(25) Private,#2: c_address VARCHAR(40) Private,#3: c_nationkey INTEGER Public,#4: c_phone CHAR(15) Private,#5: c_acctbal DECIMAL(19, 0) Private,#6: c_mktsegment CHAR(10) Private,#7: c_comment VARCHAR(117) Private)\n"
          				 + "                                LogicalProject-LocalObliviousPartitioned, schema:(#0: o_orderkey INTEGER Public,#1: o_custkey INTEGER Private)\n"
          				 + "                                    LogicalFilter-LocalObliviousPartitioned, schema:(#0: o_orderkey INTEGER Public,#1: o_custkey INTEGER Private,#2: o_orderdate DATE Private)\n"
          				 + "                                        LogicalProject-LocalClearPartitioned, schema:(#0: o_orderkey INTEGER Public,#1: o_custkey INTEGER Private,#2: o_orderdate DATE Private)\n"
          				 + "                                            JdbcTableScan-LocalClearPartitioned, schema:(#0: o_orderkey INTEGER Public,#1: o_custkey INTEGER Private,#2: o_orderstatus CHAR(1) Private,#3: o_totalprice DECIMAL(19, 0) Private,#4: o_orderdate DATE Private,#5: o_orderpriority CHAR(15) Private,#6: o_clerk CHAR(15) Private,#7: o_shippriority INTEGER Private,#8: o_comment VARCHAR(79) Private)\n"
          				 + "                            LogicalProject-LocalClearPartitioned, schema:(#0: l_orderkey INTEGER Private,#1: l_suppkey INTEGER Private,#2: l_extendedprice DECIMAL(19, 0) Private,#3: l_discount DECIMAL(19, 0) Private)\n"
          				 + "                                JdbcTableScan-LocalClearPartitioned, schema:(#0: l_orderkey INTEGER Private,#1: l_partkey INTEGER Private,#2: l_suppkey INTEGER Private,#3: l_linenumber INTEGER Private,#4: l_quantity DECIMAL(19, 0) Private,#5: l_extendedprice DECIMAL(19, 0) Private,#6: l_discount DECIMAL(19, 0) Private,#7: l_tax DECIMAL(19, 0) Private,#8: l_returnflag CHAR(1) Private,#9: l_linestatus CHAR(1) Private,#10: l_shipdate DATE Private,#11: l_commitdate DATE Private,#12: l_receiptdate DATE Private,#13: l_shipinstruct CHAR(25) Private,#14: l_shipmode CHAR(10) Private,#15: l_comment VARCHAR(44) Private)\n"
          				 + "                        LogicalProject-LocalClearPartitioned, schema:(#0: s_suppkey INTEGER Private,#1: s_nationkey INTEGER Public)\n"
          				 + "                            JdbcTableScan-LocalClearPartitioned, schema:(#0: s_suppkey INTEGER Private,#1: s_name CHAR(25) Public,#2: s_address VARCHAR(40) Private,#3: s_nationkey INTEGER Public,#4: s_phone CHAR(15) Private,#5: s_acctbal DECIMAL(19, 0) Private,#6: s_comment VARCHAR(101) Private)\n"
          				 + "                    LogicalProject-LocalClearReplicated, schema:(#0: n_nationkey INTEGER Public,#1: n_name CHAR(25) Public,#2: n_regionkey INTEGER Public)\n"
          				 + "                        JdbcTableScan-LocalClearReplicated, schema:(#0: n_nationkey INTEGER Public,#1: n_name CHAR(25) Public,#2: n_regionkey INTEGER Public,#3: n_comment VARCHAR(152) Public)\n"
          				 + "                LogicalProject-LocalClearReplicated, schema:(#0: r_regionkey INTEGER Public)\n"
          				 + "                    LogicalFilter-LocalClearReplicated, schema:(#0: r_regionkey INTEGER Public,#1: r_name CHAR(25) Public)\n"
          				 + "                        LogicalProject-LocalClearReplicated, schema:(#0: r_regionkey INTEGER Public,#1: r_name CHAR(25) Public)\n"
          				 + "                            JdbcTableScan-LocalClearReplicated, schema:(#0: r_regionkey INTEGER Public,#1: r_name CHAR(25) Public,#2: r_comment VARCHAR(152) Public)\n",
          								 
          						// 06
          								"LogicalAggregate-DistributedOblivious, schema:(#0: revenue DECIMAL(19, 0) Private)\n"
          								 + "    LogicalProject-LocalObliviousPartitioned, schema:(#0: $f0 DECIMAL(19, 0) Private)\n"
          								 + "        LogicalFilter-LocalObliviousPartitioned, schema:(#0: l_quantity DECIMAL(19, 0) Private,#1: l_extendedprice DECIMAL(19, 0) Private,#2: l_discount DECIMAL(19, 0) Private,#3: l_shipdate DATE Private)\n"
          								 + "            LogicalProject-LocalClearPartitioned, schema:(#0: l_quantity DECIMAL(19, 0) Private,#1: l_extendedprice DECIMAL(19, 0) Private,#2: l_discount DECIMAL(19, 0) Private,#3: l_shipdate DATE Private)\n"
          								 + "                JdbcTableScan-LocalClearPartitioned, schema:(#0: l_orderkey INTEGER Private,#1: l_partkey INTEGER Private,#2: l_suppkey INTEGER Private,#3: l_linenumber INTEGER Private,#4: l_quantity DECIMAL(19, 0) Private,#5: l_extendedprice DECIMAL(19, 0) Private,#6: l_discount DECIMAL(19, 0) Private,#7: l_tax DECIMAL(19, 0) Private,#8: l_returnflag CHAR(1) Private,#9: l_linestatus CHAR(1) Private,#10: l_shipdate DATE Private,#11: l_commitdate DATE Private,#12: l_receiptdate DATE Private,#13: l_shipinstruct CHAR(25) Private,#14: l_shipmode CHAR(10) Private,#15: l_comment VARCHAR(44) Private)\n",

          								 
          										 // 07
          										 "LogicalSort-Secure, schema:(#0: supp_nation CHAR(25) Public,#1: cust_nation CHAR(25) Public,#2: l_year BIGINT Private,#3: revenue DECIMAL(19, 0) Private)\n"
          												 + "    LogicalAggregate-Secure, schema:(#0: supp_nation CHAR(25) Public,#1: cust_nation CHAR(25) Public,#2: l_year BIGINT Private,#3: revenue DECIMAL(19, 0) Private), slice key: [#0: supp_nation CHAR(25) Public, #1: cust_nation CHAR(25) Public]\n"
          												 + "        LogicalProject-Secure, schema:(#0: supp_nation CHAR(25) Public,#1: cust_nation CHAR(25) Public,#2: l_year BIGINT Private,#3: volume DECIMAL(19, 0) Private)\n"
          												 + "            LogicalJoin-Secure, schema:(#0: s_suppkey INTEGER Public,#1: s_nationkey BIGINT Public,#2: l_extendedprice DECIMAL(19, 0) Private,#3: l_discount DECIMAL(19, 0) Private,#4: l_year BIGINT Private,#5: l_suppkey BIGINT Private,#6: l_orderkey BIGINT Private,#7: o_orderkey INTEGER Public,#8: o_custkey BIGINT Private,#9: c_custkey INTEGER Public,#10: c_nationkey BIGINT Public,#11: n_nationkey INTEGER Public,#12: n_name CHAR(25) Public,#13: n_nationkey0 INTEGER Public,#14: n_name0 CHAR(25) Public)\n"
          												 + "                LogicalJoin-Secure, schema:(#0: s_suppkey INTEGER Public,#1: s_nationkey BIGINT Public,#2: l_extendedprice DECIMAL(19, 0) Private,#3: l_discount DECIMAL(19, 0) Private,#4: l_year BIGINT Private,#5: l_suppkey BIGINT Private,#6: l_orderkey BIGINT Private,#7: o_orderkey INTEGER Public,#8: o_custkey BIGINT Private,#9: c_custkey INTEGER Public,#10: c_nationkey BIGINT Public,#11: n_nationkey INTEGER Public,#12: n_name CHAR(25) Public)\n"
          												 + "                    LogicalJoin-Secure, schema:(#0: s_suppkey INTEGER Public,#1: s_nationkey BIGINT Public,#2: l_extendedprice DECIMAL(19, 0) Private,#3: l_discount DECIMAL(19, 0) Private,#4: l_year BIGINT Private,#5: l_suppkey BIGINT Private,#6: l_orderkey BIGINT Private,#7: o_orderkey INTEGER Public,#8: o_custkey BIGINT Private,#9: c_custkey INTEGER Public,#10: c_nationkey BIGINT Public)\n"
          												 + "                        LogicalJoin-Secure, schema:(#0: s_suppkey INTEGER Public,#1: s_nationkey BIGINT Public,#2: l_extendedprice DECIMAL(19, 0) Private,#3: l_discount DECIMAL(19, 0) Private,#4: l_year BIGINT Private,#5: l_suppkey BIGINT Private,#6: l_orderkey BIGINT Private,#7: o_orderkey INTEGER Public,#8: o_custkey BIGINT Private)\n"
          												 + "                            LogicalJoin-Secure, schema:(#0: s_suppkey INTEGER Public,#1: s_nationkey BIGINT Public,#2: l_extendedprice DECIMAL(19, 0) Private,#3: l_discount DECIMAL(19, 0) Private,#4: l_year BIGINT Private,#5: l_suppkey BIGINT Private,#6: l_orderkey BIGINT Private)\n"
          												 + "                                LogicalProject-Plain, schema:(#0: s_suppkey INTEGER Public,#1: s_nationkey BIGINT Public)\n"
          												 + "                                    JdbcTableScan-Plain, schema:(#0: s_suppkey INTEGER Public,#1: s_name CHAR(25) Public,#2: s_address VARCHAR(40) Private,#3: s_nationkey BIGINT Public,#4: s_phone CHAR(15) Private,#5: s_acctbal DECIMAL(19, 0) Private,#6: s_comment VARCHAR(101) Private)\n"
          												 + "                                LogicalProject-Plain, schema:(#0: l_extendedprice DECIMAL(19, 0) Private,#1: l_discount DECIMAL(19, 0) Private,#2: l_year BIGINT Private,#3: l_suppkey BIGINT Private,#4: l_orderkey BIGINT Private)\n"
          												 + "                                    LogicalFilter-Plain, schema:(#0: l_orderkey BIGINT Private,#1: l_suppkey BIGINT Private,#2: l_extendedprice DECIMAL(19, 0) Private,#3: l_discount DECIMAL(19, 0) Private,#4: l_shipdate DATE Private)\n"
          												 + "                                        LogicalProject-Plain, schema:(#0: l_orderkey BIGINT Private,#1: l_suppkey BIGINT Private,#2: l_extendedprice DECIMAL(19, 0) Private,#3: l_discount DECIMAL(19, 0) Private,#4: l_shipdate DATE Private)\n"
          												 + "                                            JdbcTableScan-Plain, schema:(#0: l_orderkey BIGINT Private,#1: l_partkey BIGINT Private,#2: l_suppkey BIGINT Private,#3: l_linenumber INTEGER Private,#4: l_quantity DECIMAL(19, 0) Private,#5: l_extendedprice DECIMAL(19, 0) Private,#6: l_discount DECIMAL(19, 0) Private,#7: l_tax DECIMAL(19, 0) Private,#8: l_returnflag CHAR(1) Private,#9: l_linestatus CHAR(1) Private,#10: l_shipdate DATE Private,#11: l_commitdate DATE Private,#12: l_receiptdate DATE Private,#13: l_shipinstruct CHAR(25) Private,#14: l_shipmode CHAR(10) Private,#15: l_comment VARCHAR(44) Private)\n"
          												 + "                            LogicalProject-Plain, schema:(#0: o_orderkey INTEGER Public,#1: o_custkey BIGINT Private)\n"
          												 + "                                JdbcTableScan-Plain, schema:(#0: o_orderkey INTEGER Public,#1: o_custkey BIGINT Private,#2: o_orderstatus CHAR(1) Private,#3: o_totalprice DECIMAL(19, 0) Private,#4: o_orderdate DATE Private,#5: o_orderpriority CHAR(15) Private,#6: o_clerk CHAR(15) Private,#7: o_shippriority INTEGER Private,#8: o_comment VARCHAR(79) Private)\n"
          												 + "                        LogicalProject-Plain, schema:(#0: c_custkey INTEGER Public,#1: c_nationkey BIGINT Public)\n"
          												 + "                            JdbcTableScan-Plain, schema:(#0: c_custkey INTEGER Public,#1: c_name VARCHAR(25) Private,#2: c_address VARCHAR(40) Private,#3: c_nationkey BIGINT Public,#4: c_phone CHAR(15) Private,#5: c_acctbal DECIMAL(19, 0) Private,#6: c_mktsegment CHAR(10) Private,#7: c_comment VARCHAR(117) Private)\n"
          												 + "                    LogicalProject-Plain, schema:(#0: n_nationkey INTEGER Public,#1: n_name CHAR(25) Public)\n"
          												 + "                        JdbcTableScan-Plain, schema:(#0: n_nationkey INTEGER Public,#1: n_name CHAR(25) Public,#2: n_regionkey BIGINT Public,#3: n_comment VARCHAR(152) Public)\n"
          												 + "                LogicalProject-Plain, schema:(#0: n_nationkey INTEGER Public,#1: n_name CHAR(25) Public)\n"
          												 + "                    JdbcTableScan-Plain, schema:(#0: n_nationkey INTEGER Public,#1: n_name CHAR(25) Public,#2: n_regionkey BIGINT Public,#3: n_comment VARCHAR(152) Public)\n",
          												 
          											     //  08
          											     "LogicalProject-Secure, schema:(#0: o_year BIGINT Private,#1: mkt_share DECIMAL(19, 0) Private)\n"
 + "    LogicalSort-Secure, schema:(#0: o_year BIGINT Private,#1: $f1 DECIMAL(19, 0) Private,#2: $f2 DECIMAL(19, 0) Private)\n"
 + "        LogicalAggregate-Secure, schema:(#0: o_year BIGINT Private,#1: $f1 DECIMAL(19, 0) Private,#2: $f2 DECIMAL(19, 0) Private)\n"
 + "            LogicalProject-Secure, schema:(#0: o_year BIGINT Private,#1: $f1 DECIMAL(19, 0) Private,#2: volume DECIMAL(19, 0) Private)\n"
 + "                LogicalJoin-Plain, schema:(#0: p_partkey INTEGER Public,#1: s_suppkey INTEGER Public,#2: s_nationkey BIGINT Public,#3: l_orderkey BIGINT Private,#4: l_partkey BIGINT Private,#5: l_suppkey BIGINT Private,#6: l_extendedprice DECIMAL(19, 0) Private,#7: l_discount DECIMAL(19, 0) Private,#8: o_orderkey INTEGER Public,#9: o_custkey BIGINT Private,#10: o_year BIGINT Private,#11: c_custkey INTEGER Public,#12: c_nationkey BIGINT Public,#13: n_nationkey INTEGER Public,#14: n_regionkey BIGINT Public,#15: n_nationkey0 INTEGER Public,#16: nation_check BOOLEAN Public,#17: r_regionkey INTEGER Public)\n"
 + "                    LogicalJoin-Plain, schema:(#0: p_partkey INTEGER Public,#1: s_suppkey INTEGER Public,#2: s_nationkey BIGINT Public,#3: l_orderkey BIGINT Private,#4: l_partkey BIGINT Private,#5: l_suppkey BIGINT Private,#6: l_extendedprice DECIMAL(19, 0) Private,#7: l_discount DECIMAL(19, 0) Private,#8: o_orderkey INTEGER Public,#9: o_custkey BIGINT Private,#10: o_year BIGINT Private,#11: c_custkey INTEGER Public,#12: c_nationkey BIGINT Public,#13: n_nationkey INTEGER Public,#14: n_regionkey BIGINT Public,#15: n_nationkey0 INTEGER Public,#16: nation_check BOOLEAN Public)\n"
 + "                        LogicalJoin-Plain, schema:(#0: p_partkey INTEGER Public,#1: s_suppkey INTEGER Public,#2: s_nationkey BIGINT Public,#3: l_orderkey BIGINT Private,#4: l_partkey BIGINT Private,#5: l_suppkey BIGINT Private,#6: l_extendedprice DECIMAL(19, 0) Private,#7: l_discount DECIMAL(19, 0) Private,#8: o_orderkey INTEGER Public,#9: o_custkey BIGINT Private,#10: o_year BIGINT Private,#11: c_custkey INTEGER Public,#12: c_nationkey BIGINT Public,#13: n_nationkey INTEGER Public,#14: n_regionkey BIGINT Public)\n"
 + "                            LogicalJoin-Secure, schema:(#0: p_partkey INTEGER Public,#1: s_suppkey INTEGER Public,#2: s_nationkey BIGINT Public,#3: l_orderkey BIGINT Private,#4: l_partkey BIGINT Private,#5: l_suppkey BIGINT Private,#6: l_extendedprice DECIMAL(19, 0) Private,#7: l_discount DECIMAL(19, 0) Private,#8: o_orderkey INTEGER Public,#9: o_custkey BIGINT Private,#10: o_year BIGINT Private,#11: c_custkey INTEGER Public,#12: c_nationkey BIGINT Public)\n"
 + "                                LogicalJoin-Secure, schema:(#0: p_partkey INTEGER Public,#1: s_suppkey INTEGER Public,#2: s_nationkey BIGINT Public,#3: l_orderkey BIGINT Private,#4: l_partkey BIGINT Private,#5: l_suppkey BIGINT Private,#6: l_extendedprice DECIMAL(19, 0) Private,#7: l_discount DECIMAL(19, 0) Private,#8: o_orderkey INTEGER Public,#9: o_custkey BIGINT Private,#10: o_year BIGINT Private)\n"
 + "                                    LogicalJoin-Secure, schema:(#0: p_partkey INTEGER Public,#1: s_suppkey INTEGER Public,#2: s_nationkey BIGINT Public,#3: l_orderkey BIGINT Private,#4: l_partkey BIGINT Private,#5: l_suppkey BIGINT Private,#6: l_extendedprice DECIMAL(19, 0) Private,#7: l_discount DECIMAL(19, 0) Private)\n"
 + "                                        LogicalJoin-Plain, schema:(#0: p_partkey INTEGER Public,#1: s_suppkey INTEGER Public,#2: s_nationkey BIGINT Public)\n"
 + "                                            LogicalProject-Plain, schema:(#0: p_partkey INTEGER Public)\n"
 + "                                                LogicalFilter-Plain, schema:(#0: p_partkey INTEGER Public,#1: p_type VARCHAR(25) Public)\n"
 + "                                                    LogicalProject-Plain, schema:(#0: p_partkey INTEGER Public,#1: p_type VARCHAR(25) Public)\n"
 + "                                                        JdbcTableScan-Plain, schema:(#0: p_partkey INTEGER Public,#1: p_name VARCHAR(55) Public,#2: p_mfgr CHAR(25) Public,#3: p_brand CHAR(10) Public,#4: p_type VARCHAR(25) Public,#5: p_size INTEGER Public,#6: p_container CHAR(10) Public,#7: p_retailprice DECIMAL(19, 0) Public,#8: p_comment VARCHAR(23) Public)\n"
 + "                                            LogicalProject-Plain, schema:(#0: s_suppkey INTEGER Public,#1: s_nationkey BIGINT Public)\n"
 + "                                                JdbcTableScan-Plain, schema:(#0: s_suppkey INTEGER Public,#1: s_name CHAR(25) Public,#2: s_address VARCHAR(40) Private,#3: s_nationkey BIGINT Public,#4: s_phone CHAR(15) Private,#5: s_acctbal DECIMAL(19, 0) Private,#6: s_comment VARCHAR(101) Private)\n"
 + "                                        LogicalProject-Plain, schema:(#0: l_orderkey BIGINT Private,#1: l_partkey BIGINT Private,#2: l_suppkey BIGINT Private,#3: l_extendedprice DECIMAL(19, 0) Private,#4: l_discount DECIMAL(19, 0) Private)\n"
 + "                                            JdbcTableScan-Plain, schema:(#0: l_orderkey BIGINT Private,#1: l_partkey BIGINT Private,#2: l_suppkey BIGINT Private,#3: l_linenumber INTEGER Private,#4: l_quantity DECIMAL(19, 0) Private,#5: l_extendedprice DECIMAL(19, 0) Private,#6: l_discount DECIMAL(19, 0) Private,#7: l_tax DECIMAL(19, 0) Private,#8: l_returnflag CHAR(1) Private,#9: l_linestatus CHAR(1) Private,#10: l_shipdate DATE Private,#11: l_commitdate DATE Private,#12: l_receiptdate DATE Private,#13: l_shipinstruct CHAR(25) Private,#14: l_shipmode CHAR(10) Private,#15: l_comment VARCHAR(44) Private)\n"
 + "                                    LogicalProject-Plain, schema:(#0: o_orderkey INTEGER Public,#1: o_custkey BIGINT Private,#2: o_year BIGINT Private)\n"
 + "                                        LogicalFilter-Plain, schema:(#0: o_orderkey INTEGER Public,#1: o_custkey BIGINT Private,#2: o_orderdate DATE Private)\n"
 + "                                            LogicalProject-Plain, schema:(#0: o_orderkey INTEGER Public,#1: o_custkey BIGINT Private,#2: o_orderdate DATE Private)\n"
 + "                                                JdbcTableScan-Plain, schema:(#0: o_orderkey INTEGER Public,#1: o_custkey BIGINT Private,#2: o_orderstatus CHAR(1) Private,#3: o_totalprice DECIMAL(19, 0) Private,#4: o_orderdate DATE Private,#5: o_orderpriority CHAR(15) Private,#6: o_clerk CHAR(15) Private,#7: o_shippriority INTEGER Private,#8: o_comment VARCHAR(79) Private)\n"
 + "                                LogicalProject-Plain, schema:(#0: c_custkey INTEGER Public,#1: c_nationkey BIGINT Public)\n"
 + "                                    JdbcTableScan-Plain, schema:(#0: c_custkey INTEGER Public,#1: c_name VARCHAR(25) Private,#2: c_address VARCHAR(40) Private,#3: c_nationkey BIGINT Public,#4: c_phone CHAR(15) Private,#5: c_acctbal DECIMAL(19, 0) Private,#6: c_mktsegment CHAR(10) Private,#7: c_comment VARCHAR(117) Private)\n"
 + "                            LogicalProject-Plain, schema:(#0: n_nationkey INTEGER Public,#1: n_regionkey BIGINT Public)\n"
 + "                                JdbcTableScan-Plain, schema:(#0: n_nationkey INTEGER Public,#1: n_name CHAR(25) Public,#2: n_regionkey BIGINT Public,#3: n_comment VARCHAR(152) Public)\n"
 + "                        LogicalProject-Plain, schema:(#0: n_nationkey INTEGER Public,#1: nation_check BOOLEAN Public)\n"
 + "                            JdbcTableScan-Plain, schema:(#0: n_nationkey INTEGER Public,#1: n_name CHAR(25) Public,#2: n_regionkey BIGINT Public,#3: n_comment VARCHAR(152) Public)\n"
 + "                    LogicalProject-Plain, schema:(#0: r_regionkey INTEGER Public)\n"
 + "                        LogicalFilter-Plain, schema:(#0: r_regionkey INTEGER Public,#1: r_name CHAR(25) Public)\n"
 + "                            LogicalProject-Plain, schema:(#0: r_regionkey INTEGER Public,#1: r_name CHAR(25) Public)\n"
 + "                                JdbcTableScan-Plain, schema:(#0: r_regionkey INTEGER Public,#1: r_name CHAR(25) Public,#2: r_comment VARCHAR(152) Public)\n",

          											     //  09
 "LogicalSort-Secure, schema:(#0: nation CHAR(25) Public,#1: o_year BIGINT Private,#2: sum_profit DECIMAL(19, 0) Private)\n"
 + "    LogicalAggregate-Secure, schema:(#0: nation CHAR(25) Public,#1: o_year BIGINT Private,#2: sum_profit DECIMAL(19, 0) Private), slice key: [#0: nation CHAR(25) Public]\n"
 + "        LogicalProject-Secure, schema:(#0: nation CHAR(25) Public,#1: o_year BIGINT Private,#2: amount DECIMAL(19, 0) Private)\n"
 + "            LogicalJoin-Plain, schema:(#0: p_partkey INTEGER Public,#1: s_suppkey INTEGER Public,#2: s_nationkey BIGINT Public,#3: l_orderkey BIGINT Private,#4: l_partkey BIGINT Private,#5: l_suppkey BIGINT Private,#6: l_quantity DECIMAL(19, 0) Private,#7: l_extendedprice DECIMAL(19, 0) Private,#8: l_discount DECIMAL(19, 0) Private,#9: ps_partkey BIGINT Public,#10: ps_suppkey BIGINT Public,#11: ps_supplycost DECIMAL(19, 0) Public,#12: o_year BIGINT Private,#13: o_orderkey INTEGER Public,#14: n_nationkey INTEGER Public,#15: n_name CHAR(25) Public)\n"
 + "                LogicalJoin-Secure, schema:(#0: p_partkey INTEGER Public,#1: s_suppkey INTEGER Public,#2: s_nationkey BIGINT Public,#3: l_orderkey BIGINT Private,#4: l_partkey BIGINT Private,#5: l_suppkey BIGINT Private,#6: l_quantity DECIMAL(19, 0) Private,#7: l_extendedprice DECIMAL(19, 0) Private,#8: l_discount DECIMAL(19, 0) Private,#9: ps_partkey BIGINT Public,#10: ps_suppkey BIGINT Public,#11: ps_supplycost DECIMAL(19, 0) Public,#12: o_year BIGINT Private,#13: o_orderkey INTEGER Public)\n"
 + "                    LogicalJoin-Secure, schema:(#0: p_partkey INTEGER Public,#1: s_suppkey INTEGER Public,#2: s_nationkey BIGINT Public,#3: l_orderkey BIGINT Private,#4: l_partkey BIGINT Private,#5: l_suppkey BIGINT Private,#6: l_quantity DECIMAL(19, 0) Private,#7: l_extendedprice DECIMAL(19, 0) Private,#8: l_discount DECIMAL(19, 0) Private,#9: ps_partkey BIGINT Public,#10: ps_suppkey BIGINT Public,#11: ps_supplycost DECIMAL(19, 0) Public)\n"
 + "                        LogicalJoin-Secure, schema:(#0: p_partkey INTEGER Public,#1: s_suppkey INTEGER Public,#2: s_nationkey BIGINT Public,#3: l_orderkey BIGINT Private,#4: l_partkey BIGINT Private,#5: l_suppkey BIGINT Private,#6: l_quantity DECIMAL(19, 0) Private,#7: l_extendedprice DECIMAL(19, 0) Private,#8: l_discount DECIMAL(19, 0) Private)\n"
 + "                            LogicalJoin-Plain, schema:(#0: p_partkey INTEGER Public,#1: s_suppkey INTEGER Public,#2: s_nationkey BIGINT Public)\n"
 + "                                LogicalProject-Plain, schema:(#0: p_partkey INTEGER Public)\n"
 + "                                    LogicalFilter-Plain, schema:(#0: p_partkey INTEGER Public,#1: p_name VARCHAR(55) Public)\n"
 + "                                        LogicalProject-Plain, schema:(#0: p_partkey INTEGER Public,#1: p_name VARCHAR(55) Public)\n"
 + "                                            JdbcTableScan-Plain, schema:(#0: p_partkey INTEGER Public,#1: p_name VARCHAR(55) Public,#2: p_mfgr CHAR(25) Public,#3: p_brand CHAR(10) Public,#4: p_type VARCHAR(25) Public,#5: p_size INTEGER Public,#6: p_container CHAR(10) Public,#7: p_retailprice DECIMAL(19, 0) Public,#8: p_comment VARCHAR(23) Public)\n"
 + "                                LogicalProject-Plain, schema:(#0: s_suppkey INTEGER Public,#1: s_nationkey BIGINT Public)\n"
 + "                                    JdbcTableScan-Plain, schema:(#0: s_suppkey INTEGER Public,#1: s_name CHAR(25) Public,#2: s_address VARCHAR(40) Private,#3: s_nationkey BIGINT Public,#4: s_phone CHAR(15) Private,#5: s_acctbal DECIMAL(19, 0) Private,#6: s_comment VARCHAR(101) Private)\n"
 + "                            LogicalProject-Plain, schema:(#0: l_orderkey BIGINT Private,#1: l_partkey BIGINT Private,#2: l_suppkey BIGINT Private,#3: l_quantity DECIMAL(19, 0) Private,#4: l_extendedprice DECIMAL(19, 0) Private,#5: l_discount DECIMAL(19, 0) Private)\n"
 + "                                JdbcTableScan-Plain, schema:(#0: l_orderkey BIGINT Private,#1: l_partkey BIGINT Private,#2: l_suppkey BIGINT Private,#3: l_linenumber INTEGER Private,#4: l_quantity DECIMAL(19, 0) Private,#5: l_extendedprice DECIMAL(19, 0) Private,#6: l_discount DECIMAL(19, 0) Private,#7: l_tax DECIMAL(19, 0) Private,#8: l_returnflag CHAR(1) Private,#9: l_linestatus CHAR(1) Private,#10: l_shipdate DATE Private,#11: l_commitdate DATE Private,#12: l_receiptdate DATE Private,#13: l_shipinstruct CHAR(25) Private,#14: l_shipmode CHAR(10) Private,#15: l_comment VARCHAR(44) Private)\n"
 + "                        LogicalProject-Plain, schema:(#0: ps_partkey BIGINT Public,#1: ps_suppkey BIGINT Public,#2: ps_supplycost DECIMAL(19, 0) Public)\n"
 + "                            JdbcTableScan-Plain, schema:(#0: ps_partkey BIGINT Public,#1: ps_suppkey BIGINT Public,#2: ps_availqty INTEGER Private,#3: ps_supplycost DECIMAL(19, 0) Public,#4: ps_comment VARCHAR(199) Private)\n"
 + "                    LogicalProject-Plain, schema:(#0: o_year BIGINT Private,#1: o_orderkey INTEGER Public)\n"
 + "                        JdbcTableScan-Plain, schema:(#0: o_orderkey INTEGER Public,#1: o_custkey BIGINT Private,#2: o_orderstatus CHAR(1) Private,#3: o_totalprice DECIMAL(19, 0) Private,#4: o_orderdate DATE Private,#5: o_orderpriority CHAR(15) Private,#6: o_clerk CHAR(15) Private,#7: o_shippriority INTEGER Private,#8: o_comment VARCHAR(79) Private)\n"
 + "                LogicalProject-Plain, schema:(#0: n_nationkey INTEGER Public,#1: n_name CHAR(25) Public)\n"
 + "                    JdbcTableScan-Plain, schema:(#0: n_nationkey INTEGER Public,#1: n_name CHAR(25) Public,#2: n_regionkey BIGINT Public,#3: n_comment VARCHAR(152) Public)\n"
,

          											     //  10
 "LogicalProject-Secure, schema:(#0: c_custkey INTEGER Public,#1: c_name VARCHAR(25) Private,#2: revenue DECIMAL(19, 0) Private,#3: c_acctbal DECIMAL(19, 0) Private,#4: n_name CHAR(25) Public,#5: c_address VARCHAR(40) Private,#6: c_phone CHAR(15) Private,#7: c_comment VARCHAR(117) Private)\n"
+ "    LogicalSort-Secure, schema:(#0: c_custkey INTEGER Public,#1: c_name VARCHAR(25) Private,#2: c_acctbal DECIMAL(19, 0) Private,#3: c_phone CHAR(15) Private,#4: n_name CHAR(25) Public,#5: c_address VARCHAR(40) Private,#6: c_comment VARCHAR(117) Private,#7: revenue DECIMAL(19, 0) Private)\n"
+ "        LogicalAggregate-Secure, schema:(#0: c_custkey INTEGER Public,#1: c_name VARCHAR(25) Private,#2: c_acctbal DECIMAL(19, 0) Private,#3: c_phone CHAR(15) Private,#4: n_name CHAR(25) Public,#5: c_address VARCHAR(40) Private,#6: c_comment VARCHAR(117) Private,#7: revenue DECIMAL(19, 0) Private), slice key: [#0: c_custkey INTEGER Public, #4: n_name CHAR(25) Public]\n"
+ "            LogicalProject-Secure, schema:(#0: c_custkey INTEGER Public,#1: c_name VARCHAR(25) Private,#2: c_acctbal DECIMAL(19, 0) Private,#3: c_phone CHAR(15) Private,#4: n_name CHAR(25) Public,#5: c_address VARCHAR(40) Private,#6: c_comment VARCHAR(117) Private,#7: $f7 DECIMAL(19, 0) Private)\n"
+ "                LogicalJoin-Plain, schema:(#0: c_custkey INTEGER Public,#1: c_name VARCHAR(25) Private,#2: c_address VARCHAR(40) Private,#3: c_nationkey BIGINT Public,#4: c_phone CHAR(15) Private,#5: c_acctbal DECIMAL(19, 0) Private,#6: c_comment VARCHAR(117) Private,#7: o_orderkey INTEGER Public,#8: o_custkey BIGINT Private,#9: l_orderkey BIGINT Private,#10: l_extendedprice DECIMAL(19, 0) Private,#11: l_discount DECIMAL(19, 0) Private,#12: n_nationkey INTEGER Public,#13: n_name CHAR(25) Public)\n"
+ "                    LogicalJoin-Secure, schema:(#0: c_custkey INTEGER Public,#1: c_name VARCHAR(25) Private,#2: c_address VARCHAR(40) Private,#3: c_nationkey BIGINT Public,#4: c_phone CHAR(15) Private,#5: c_acctbal DECIMAL(19, 0) Private,#6: c_comment VARCHAR(117) Private,#7: o_orderkey INTEGER Public,#8: o_custkey BIGINT Private,#9: l_orderkey BIGINT Private,#10: l_extendedprice DECIMAL(19, 0) Private,#11: l_discount DECIMAL(19, 0) Private)\n"
+ "                        LogicalJoin-Secure, schema:(#0: c_custkey INTEGER Public,#1: c_name VARCHAR(25) Private,#2: c_address VARCHAR(40) Private,#3: c_nationkey BIGINT Public,#4: c_phone CHAR(15) Private,#5: c_acctbal DECIMAL(19, 0) Private,#6: c_comment VARCHAR(117) Private,#7: o_orderkey INTEGER Public,#8: o_custkey BIGINT Private)\n"
+ "                            LogicalProject-Plain, schema:(#0: c_custkey INTEGER Public,#1: c_name VARCHAR(25) Private,#2: c_address VARCHAR(40) Private,#3: c_nationkey BIGINT Public,#4: c_phone CHAR(15) Private,#5: c_acctbal DECIMAL(19, 0) Private,#6: c_comment VARCHAR(117) Private)\n"
+ "                                JdbcTableScan-Plain, schema:(#0: c_custkey INTEGER Public,#1: c_name VARCHAR(25) Private,#2: c_address VARCHAR(40) Private,#3: c_nationkey BIGINT Public,#4: c_phone CHAR(15) Private,#5: c_acctbal DECIMAL(19, 0) Private,#6: c_mktsegment CHAR(10) Private,#7: c_comment VARCHAR(117) Private)\n"
+ "                            LogicalProject-Plain, schema:(#0: o_orderkey INTEGER Public,#1: o_custkey BIGINT Private)\n"
+ "                                LogicalFilter-Plain, schema:(#0: o_orderkey INTEGER Public,#1: o_custkey BIGINT Private,#2: o_orderdate DATE Private)\n"
+ "                                    LogicalProject-Plain, schema:(#0: o_orderkey INTEGER Public,#1: o_custkey BIGINT Private,#2: o_orderdate DATE Private)\n"
+ "                                        JdbcTableScan-Plain, schema:(#0: o_orderkey INTEGER Public,#1: o_custkey BIGINT Private,#2: o_orderstatus CHAR(1) Private,#3: o_totalprice DECIMAL(19, 0) Private,#4: o_orderdate DATE Private,#5: o_orderpriority CHAR(15) Private,#6: o_clerk CHAR(15) Private,#7: o_shippriority INTEGER Private,#8: o_comment VARCHAR(79) Private)\n"
+ "                        LogicalProject-Plain, schema:(#0: l_orderkey BIGINT Private,#1: l_extendedprice DECIMAL(19, 0) Private,#2: l_discount DECIMAL(19, 0) Private)\n"
+ "                            LogicalFilter-Plain, schema:(#0: l_orderkey BIGINT Private,#1: l_extendedprice DECIMAL(19, 0) Private,#2: l_discount DECIMAL(19, 0) Private,#3: l_returnflag CHAR(1) Private)\n"
+ "                                LogicalProject-Plain, schema:(#0: l_orderkey BIGINT Private,#1: l_extendedprice DECIMAL(19, 0) Private,#2: l_discount DECIMAL(19, 0) Private,#3: l_returnflag CHAR(1) Private)\n"
+ "                                    JdbcTableScan-Plain, schema:(#0: l_orderkey BIGINT Private,#1: l_partkey BIGINT Private,#2: l_suppkey BIGINT Private,#3: l_linenumber INTEGER Private,#4: l_quantity DECIMAL(19, 0) Private,#5: l_extendedprice DECIMAL(19, 0) Private,#6: l_discount DECIMAL(19, 0) Private,#7: l_tax DECIMAL(19, 0) Private,#8: l_returnflag CHAR(1) Private,#9: l_linestatus CHAR(1) Private,#10: l_shipdate DATE Private,#11: l_commitdate DATE Private,#12: l_receiptdate DATE Private,#13: l_shipinstruct CHAR(25) Private,#14: l_shipmode CHAR(10) Private,#15: l_comment VARCHAR(44) Private)\n"
+ "                    LogicalProject-Plain, schema:(#0: n_nationkey INTEGER Public,#1: n_name CHAR(25) Public)\n"
+ "                        JdbcTableScan-Plain, schema:(#0: n_nationkey INTEGER Public,#1: n_name CHAR(25) Public,#2: n_regionkey BIGINT Public,#3: n_comment VARCHAR(152) Public)\n",

          											     //  11
"LogicalProject-DistributedOblivious, schema:(#0: ps_partkey INTEGER Public,#1: val DECIMAL(19, 0) Private)\n"
+ "    LogicalSort-DistributedOblivious, schema:(#0: ps_partkey INTEGER Public,#1: val DECIMAL(19, 0) Private,#2: $f0 DECIMAL(19, 10) Private)\n"
+ "        LogicalJoin-DistributedOblivious, schema:(#0: ps_partkey INTEGER Public,#1: val DECIMAL(19, 0) Private,#2: $f0 DECIMAL(19, 10) Private), Predicate: >($1, $2)\n"
+ "            LogicalAggregate-DistributedOblivious, schema:(#0: ps_partkey INTEGER Public,#1: val DECIMAL(19, 0) Private), slice key: [#0: ps_partkey INTEGER Public]\n"
+ "                LogicalProject-LocalObliviousPartitioned, schema:(#0: ps_partkey INTEGER Public,#1: $f1 DECIMAL(19, 0) Private)\n"
+ "                    LogicalJoin-LocalObliviousPartitioned, schema:(#0: ps_partkey INTEGER Public,#1: ps_suppkey INTEGER Public,#2: ps_availqty INTEGER Private,#3: ps_supplycost DECIMAL(19, 0) Public,#4: s_suppkey INTEGER Private,#5: s_nationkey INTEGER Public,#6: n_nationkey INTEGER Public), Predicate: =($5, $6), slice key: [#5: s_nationkey INTEGER Public, #6: n_nationkey INTEGER Public]\n"
+ "                        LogicalJoin-LocalObliviousPartitioned, schema:(#0: ps_partkey INTEGER Public,#1: ps_suppkey INTEGER Public,#2: ps_availqty INTEGER Private,#3: ps_supplycost DECIMAL(19, 0) Public,#4: s_suppkey INTEGER Private,#5: s_nationkey INTEGER Public), Predicate: =($1, $4)\n"
+ "                            LogicalProject-LocalClearPartitioned, schema:(#0: ps_partkey INTEGER Public,#1: ps_suppkey INTEGER Public,#2: ps_availqty INTEGER Private,#3: ps_supplycost DECIMAL(19, 0) Public)\n"
+ "                                JdbcTableScan-LocalClearPartitioned, schema:(#0: ps_partkey INTEGER Public,#1: ps_suppkey INTEGER Public,#2: ps_availqty INTEGER Private,#3: ps_supplycost DECIMAL(19, 0) Public,#4: ps_comment VARCHAR(199) Private)\n"
+ "                            LogicalProject-LocalClearPartitioned, schema:(#0: s_suppkey INTEGER Private,#1: s_nationkey INTEGER Public)\n"
+ "                                JdbcTableScan-LocalClearPartitioned, schema:(#0: s_suppkey INTEGER Private,#1: s_name CHAR(25) Public,#2: s_address VARCHAR(40) Private,#3: s_nationkey INTEGER Public,#4: s_phone CHAR(15) Private,#5: s_acctbal DECIMAL(19, 0) Private,#6: s_comment VARCHAR(101) Private)\n"
+ "                        LogicalProject-LocalClearReplicated, schema:(#0: n_nationkey INTEGER Public)\n"
+ "                            LogicalFilter-LocalClearReplicated, schema:(#0: n_nationkey INTEGER Public,#1: n_name CHAR(25) Public)\n"
+ "                                LogicalProject-LocalClearReplicated, schema:(#0: n_nationkey INTEGER Public,#1: n_name CHAR(25) Public)\n"
+ "                                    JdbcTableScan-LocalClearReplicated, schema:(#0: n_nationkey INTEGER Public,#1: n_name CHAR(25) Public,#2: n_regionkey INTEGER Public,#3: n_comment VARCHAR(152) Public)\n"
+ "            LogicalAggregate-DistributedOblivious, schema:(#0: $f0 DECIMAL(19, 10) Private)\n"
+ "                LogicalProject-DistributedOblivious, schema:(#0: EXPR$0 DECIMAL(19, 10) Private)\n"
+ "                    LogicalAggregate-DistributedOblivious, schema:(#0: $f0 DECIMAL(19, 0) Private)\n"
+ "                        LogicalProject-LocalObliviousPartitioned, schema:(#0: $f0 DECIMAL(19, 0) Private)\n"
+ "                            LogicalJoin-LocalObliviousPartitioned, schema:(#0: ps_suppkey INTEGER Public,#1: ps_availqty INTEGER Private,#2: ps_supplycost DECIMAL(19, 0) Public,#3: s_suppkey INTEGER Private,#4: s_nationkey INTEGER Public,#5: n_nationkey INTEGER Public), Predicate: =($4, $5), slice key: [#4: s_nationkey INTEGER Public, #5: n_nationkey INTEGER Public]\n"
+ "                                LogicalJoin-LocalObliviousPartitioned, schema:(#0: ps_suppkey INTEGER Public,#1: ps_availqty INTEGER Private,#2: ps_supplycost DECIMAL(19, 0) Public,#3: s_suppkey INTEGER Private,#4: s_nationkey INTEGER Public), Predicate: =($0, $3)\n"
+ "                                    LogicalProject-LocalClearPartitioned, schema:(#0: ps_suppkey INTEGER Public,#1: ps_availqty INTEGER Private,#2: ps_supplycost DECIMAL(19, 0) Public)\n"
+ "                                        JdbcTableScan-LocalClearPartitioned, schema:(#0: ps_partkey INTEGER Public,#1: ps_suppkey INTEGER Public,#2: ps_availqty INTEGER Private,#3: ps_supplycost DECIMAL(19, 0) Public,#4: ps_comment VARCHAR(199) Private)\n"
+ "                                    LogicalProject-LocalClearPartitioned, schema:(#0: s_suppkey INTEGER Private,#1: s_nationkey INTEGER Public)\n"
+ "                                        JdbcTableScan-LocalClearPartitioned, schema:(#0: s_suppkey INTEGER Private,#1: s_name CHAR(25) Public,#2: s_address VARCHAR(40) Private,#3: s_nationkey INTEGER Public,#4: s_phone CHAR(15) Private,#5: s_acctbal DECIMAL(19, 0) Private,#6: s_comment VARCHAR(101) Private)\n"
+ "                                LogicalProject-LocalClearReplicated, schema:(#0: n_nationkey INTEGER Public)\n"
+ "                                    LogicalFilter-LocalClearReplicated, schema:(#0: n_nationkey INTEGER Public,#1: n_name CHAR(25) Public)\n"
+ "                                        LogicalProject-LocalClearReplicated, schema:(#0: n_nationkey INTEGER Public,#1: n_name CHAR(25) Public)\n"
+ "                                            JdbcTableScan-LocalClearReplicated, schema:(#0: n_nationkey INTEGER Public,#1: n_name CHAR(25) Public,#2: n_regionkey INTEGER Public,#3: n_comment VARCHAR(152) Public)\n",


          											     //  12
"LogicalSort-DistributedOblivious, schema:(#0: l_shipmode CHAR(10) Private,#1: high_line_count INTEGER Private,#2: low_line_count INTEGER Private)\n"
+ "    LogicalAggregate-DistributedOblivious, schema:(#0: l_shipmode CHAR(10) Private,#1: high_line_count INTEGER Private,#2: low_line_count INTEGER Private)\n"
+ "        LogicalProject-LocalObliviousPartitioned, schema:(#0: l_shipmode CHAR(10) Private,#1: $f1 INTEGER Private,#2: $f2 INTEGER Private)\n"
+ "            LogicalJoin-LocalObliviousPartitioned, schema:(#0: o_orderkey INTEGER Public,#1: o_orderpriority CHAR(15) Private,#2: l_orderkey INTEGER Private,#3: l_shipmode CHAR(10) Private), Predicate: =($0, $2)\n"
+ "                LogicalProject-LocalObliviousPartitioned, schema:(#0: o_orderkey INTEGER Public,#1: o_orderpriority CHAR(15) Private)\n"
+ "                    JdbcTableScan-LocalClearPartitioned, schema:(#0: o_orderkey INTEGER Public,#1: o_custkey INTEGER Private,#2: o_orderstatus CHAR(1) Private,#3: o_totalprice DECIMAL(19, 0) Private,#4: o_orderdate DATE Private,#5: o_orderpriority CHAR(15) Private,#6: o_clerk CHAR(15) Private,#7: o_shippriority INTEGER Private,#8: o_comment VARCHAR(79) Private)\n"
+ "                LogicalProject-LocalObliviousPartitioned, schema:(#0: l_orderkey INTEGER Private,#1: l_shipmode CHAR(10) Private)\n"
+ "                    LogicalFilter-LocalObliviousPartitioned, schema:(#0: l_orderkey INTEGER Private,#1: l_shipdate DATE Private,#2: l_commitdate DATE Private,#3: l_receiptdate DATE Private,#4: l_shipmode CHAR(10) Private)\n"
+ "                        LogicalProject-LocalClearPartitioned, schema:(#0: l_orderkey INTEGER Private,#1: l_shipdate DATE Private,#2: l_commitdate DATE Private,#3: l_receiptdate DATE Private,#4: l_shipmode CHAR(10) Private)\n"
+ "                            JdbcTableScan-LocalClearPartitioned, schema:(#0: l_orderkey INTEGER Private,#1: l_partkey INTEGER Private,#2: l_suppkey INTEGER Private,#3: l_linenumber INTEGER Private,#4: l_quantity DECIMAL(19, 0) Private,#5: l_extendedprice DECIMAL(19, 0) Private,#6: l_discount DECIMAL(19, 0) Private,#7: l_tax DECIMAL(19, 0) Private,#8: l_returnflag CHAR(1) Private,#9: l_linestatus CHAR(1) Private,#10: l_shipdate DATE Private,#11: l_commitdate DATE Private,#12: l_receiptdate DATE Private,#13: l_shipinstruct CHAR(25) Private,#14: l_shipmode CHAR(10) Private,#15: l_comment VARCHAR(44) Private)\n",


          											     //  13
"LogicalSort-DistributedOblivious, schema:(#0: c_count BIGINT Public,#1: custdist BIGINT Public)\n"
+ "    LogicalAggregate-DistributedOblivious, schema:(#0: c_count BIGINT Public,#1: custdist BIGINT Public)\n"
+ "        LogicalProject-DistributedOblivious, schema:(#0: c_count BIGINT Public), slice key: [#0: c_count BIGINT Public]\n"
+ "            LogicalAggregate-DistributedOblivious, schema:(#0: c_custkey INTEGER Public,#1: c_count BIGINT Public), slice key: [#0: c_custkey INTEGER Public, #1: o_orderkey INTEGER Public]\n"
+ "                LogicalProject-LocalObliviousPartitioned, schema:(#0: c_custkey INTEGER Public,#1: o_orderkey INTEGER Public)\n"
+ "                    LogicalJoin-LocalObliviousPartitioned, schema:(#0: c_custkey INTEGER Public,#1: o_orderkey INTEGER Public,#2: o_custkey INTEGER Private), Predicate: =($0, $2)\n"
+ "                        LogicalProject-LocalObliviousPartitioned, schema:(#0: c_custkey INTEGER Public)\n"
+ "                            JdbcTableScan-LocalClearPartitioned, schema:(#0: c_custkey INTEGER Public,#1: c_name VARCHAR(25) Private,#2: c_address VARCHAR(40) Private,#3: c_nationkey INTEGER Public,#4: c_phone CHAR(15) Private,#5: c_acctbal DECIMAL(19, 0) Private,#6: c_mktsegment CHAR(10) Private,#7: c_comment VARCHAR(117) Private)\n"
+ "                        LogicalProject-LocalObliviousPartitioned, schema:(#0: o_orderkey INTEGER Public,#1: o_custkey INTEGER Private)\n"
+ "                            LogicalFilter-LocalObliviousPartitioned, schema:(#0: o_orderkey INTEGER Public,#1: o_custkey INTEGER Private,#2: o_comment VARCHAR(79) Private)\n"
+ "                                LogicalProject-LocalClearPartitioned, schema:(#0: o_orderkey INTEGER Public,#1: o_custkey INTEGER Private,#2: o_comment VARCHAR(79) Private)\n"
+ "                                    JdbcTableScan-LocalClearPartitioned, schema:(#0: o_orderkey INTEGER Public,#1: o_custkey INTEGER Private,#2: o_orderstatus CHAR(1) Private,#3: o_totalprice DECIMAL(19, 0) Private,#4: o_orderdate DATE Private,#5: o_orderpriority CHAR(15) Private,#6: o_clerk CHAR(15) Private,#7: o_shippriority INTEGER Private,#8: o_comment VARCHAR(79) Private)\n",

          											     //  14
"LogicalProject-DistributedOblivious, schema:(#0: promo_revenue DECIMAL(19, 2) Private)\n"
+ "    LogicalAggregate-DistributedOblivious, schema:(#0: $f0 DECIMAL(19, 0) Private,#1: $f1 DECIMAL(19, 0) Private)\n"
+ "        LogicalProject-LocalObliviousPartitioned, schema:(#0: $f0 DECIMAL(19, 0) Private,#1: $f1 DECIMAL(19, 0) Private)\n"
+ "            LogicalJoin-LocalObliviousPartitioned, schema:(#0: l_partkey INTEGER Private,#1: l_extendedprice DECIMAL(19, 0) Private,#2: l_discount DECIMAL(19, 0) Private,#3: p_partkey INTEGER Public,#4: p_type VARCHAR(25) Public), Predicate: =($0, $3)\n"
+ "                LogicalProject-LocalObliviousPartitioned, schema:(#0: l_partkey INTEGER Private,#1: l_extendedprice DECIMAL(19, 0) Private,#2: l_discount DECIMAL(19, 0) Private)\n"
+ "                    LogicalFilter-LocalObliviousPartitioned, schema:(#0: l_partkey INTEGER Private,#1: l_extendedprice DECIMAL(19, 0) Private,#2: l_discount DECIMAL(19, 0) Private,#3: l_shipdate DATE Private)\n"
+ "                        LogicalProject-LocalClearPartitioned, schema:(#0: l_partkey INTEGER Private,#1: l_extendedprice DECIMAL(19, 0) Private,#2: l_discount DECIMAL(19, 0) Private,#3: l_shipdate DATE Private)\n"
+ "                            JdbcTableScan-LocalClearPartitioned, schema:(#0: l_orderkey INTEGER Private,#1: l_partkey INTEGER Private,#2: l_suppkey INTEGER Private,#3: l_linenumber INTEGER Private,#4: l_quantity DECIMAL(19, 0) Private,#5: l_extendedprice DECIMAL(19, 0) Private,#6: l_discount DECIMAL(19, 0) Private,#7: l_tax DECIMAL(19, 0) Private,#8: l_returnflag CHAR(1) Private,#9: l_linestatus CHAR(1) Private,#10: l_shipdate DATE Private,#11: l_commitdate DATE Private,#12: l_receiptdate DATE Private,#13: l_shipinstruct CHAR(25) Private,#14: l_shipmode CHAR(10) Private,#15: l_comment VARCHAR(44) Private)\n"
+ "                LogicalProject-LocalClearReplicated, schema:(#0: p_partkey INTEGER Public,#1: p_type VARCHAR(25) Public)\n"
+ "                    JdbcTableScan-LocalClearReplicated, schema:(#0: p_partkey INTEGER Public,#1: p_name VARCHAR(55) Public,#2: p_mfgr CHAR(25) Public,#3: p_brand CHAR(10) Public,#4: p_type VARCHAR(25) Public,#5: p_size INTEGER Public,#6: p_container CHAR(10) Public,#7: p_retailprice DECIMAL(19, 0) Public,#8: p_comment VARCHAR(23) Public)\n",

          											     //  15
"LogicalProject-Secure, schema:(#0: s_suppkey INTEGER Public,#1: s_name CHAR(25) Public,#2: s_address VARCHAR(40) Private,#3: s_phone CHAR(15) Private,#4: total_revenue DECIMAL(19, 0) Private)\n"
+ "    LogicalSort-Secure, schema:(#0: s_suppkey INTEGER Public,#1: s_name CHAR(25) Public,#2: s_address VARCHAR(40) Private,#3: s_phone CHAR(15) Private,#4: l_suppkey BIGINT Private,#5: EXPR$1 DECIMAL(19, 0) Private,#6: EXPR$0 DECIMAL(19, 0) Private)\n"
+ "        LogicalJoin-Secure, schema:(#0: s_suppkey INTEGER Public,#1: s_name CHAR(25) Public,#2: s_address VARCHAR(40) Private,#3: s_phone CHAR(15) Private,#4: l_suppkey BIGINT Private,#5: EXPR$1 DECIMAL(19, 0) Private,#6: EXPR$0 DECIMAL(19, 0) Private)\n"
+ "            LogicalJoin-Secure, schema:(#0: s_suppkey INTEGER Public,#1: s_name CHAR(25) Public,#2: s_address VARCHAR(40) Private,#3: s_phone CHAR(15) Private,#4: l_suppkey BIGINT Private,#5: EXPR$1 DECIMAL(19, 0) Private)\n"
+ "                LogicalProject-Plain, schema:(#0: s_suppkey INTEGER Public,#1: s_name CHAR(25) Public,#2: s_address VARCHAR(40) Private,#3: s_phone CHAR(15) Private)\n"
+ "                    JdbcTableScan-Plain, schema:(#0: s_suppkey INTEGER Public,#1: s_name CHAR(25) Public,#2: s_address VARCHAR(40) Private,#3: s_nationkey BIGINT Public,#4: s_phone CHAR(15) Private,#5: s_acctbal DECIMAL(19, 0) Private,#6: s_comment VARCHAR(101) Private)\n"
+ "                LogicalAggregate-Secure, schema:(#0: l_suppkey BIGINT Private,#1: EXPR$1 DECIMAL(19, 0) Private)\n"
+ "                    LogicalProject-Plain, schema:(#0: l_suppkey BIGINT Private,#1: $f1 DECIMAL(19, 0) Private)\n"
+ "                        LogicalFilter-Plain, schema:(#0: l_suppkey BIGINT Private,#1: l_extendedprice DECIMAL(19, 0) Private,#2: l_discount DECIMAL(19, 0) Private,#3: l_shipdate DATE Private)\n"
+ "                            LogicalProject-Plain, schema:(#0: l_suppkey BIGINT Private,#1: l_extendedprice DECIMAL(19, 0) Private,#2: l_discount DECIMAL(19, 0) Private,#3: l_shipdate DATE Private)\n"
+ "                                JdbcTableScan-Plain, schema:(#0: l_orderkey BIGINT Private,#1: l_partkey BIGINT Private,#2: l_suppkey BIGINT Private,#3: l_linenumber INTEGER Private,#4: l_quantity DECIMAL(19, 0) Private,#5: l_extendedprice DECIMAL(19, 0) Private,#6: l_discount DECIMAL(19, 0) Private,#7: l_tax DECIMAL(19, 0) Private,#8: l_returnflag CHAR(1) Private,#9: l_linestatus CHAR(1) Private,#10: l_shipdate DATE Private,#11: l_commitdate DATE Private,#12: l_receiptdate DATE Private,#13: l_shipinstruct CHAR(25) Private,#14: l_shipmode CHAR(10) Private,#15: l_comment VARCHAR(44) Private)\n"
+ "            LogicalAggregate-Secure, schema:(#0: EXPR$0 DECIMAL(19, 0) Private)\n"
+ "                LogicalProject-Secure, schema:(#0: EXPR$1 DECIMAL(19, 0) Private)\n"
+ "                    LogicalAggregate-Secure, schema:(#0: l_suppkey BIGINT Private,#1: EXPR$1 DECIMAL(19, 0) Private)\n"
+ "                        LogicalProject-Plain, schema:(#0: l_suppkey BIGINT Private,#1: $f1 DECIMAL(19, 0) Private)\n"
+ "                            LogicalFilter-Plain, schema:(#0: l_suppkey BIGINT Private,#1: l_extendedprice DECIMAL(19, 0) Private,#2: l_discount DECIMAL(19, 0) Private,#3: l_shipdate DATE Private)\n"
+ "                                LogicalProject-Plain, schema:(#0: l_suppkey BIGINT Private,#1: l_extendedprice DECIMAL(19, 0) Private,#2: l_discount DECIMAL(19, 0) Private,#3: l_shipdate DATE Private)\n"
+ "                                    JdbcTableScan-Plain, schema:(#0: l_orderkey BIGINT Private,#1: l_partkey BIGINT Private,#2: l_suppkey BIGINT Private,#3: l_linenumber INTEGER Private,#4: l_quantity DECIMAL(19, 0) Private,#5: l_extendedprice DECIMAL(19, 0) Private,#6: l_discount DECIMAL(19, 0) Private,#7: l_tax DECIMAL(19, 0) Private,#8: l_returnflag CHAR(1) Private,#9: l_linestatus CHAR(1) Private,#10: l_shipdate DATE Private,#11: l_commitdate DATE Private,#12: l_receiptdate DATE Private,#13: l_shipinstruct CHAR(25) Private,#14: l_shipmode CHAR(10) Private,#15: l_comment VARCHAR(44) Private)\n"
,

          											     //  16
"LogicalSort-Securexsx, schema:(#0: p_brand CHAR(10) Public,#1: p_type VARCHAR(25) Public,#2: p_size INTEGER Public,#3: supplier_cnt BIGINT Public)\n"
+ "    LogicalAggregate-Secure, schema:(#0: p_brand CHAR(10) Public,#1: p_type VARCHAR(25) Public,#2: p_size INTEGER Public,#3: supplier_cnt BIGINT Public), slice key: [#0: p_brand CHAR(10) Public, #1: p_type VARCHAR(25) Public, #2: p_size INTEGER Public, #3: ps_suppkey BIGINT Public]\n"
+ "        LogicalAggregate-Secure, schema:(#0: p_brand CHAR(10) Public,#1: p_type VARCHAR(25) Public,#2: p_size INTEGER Public,#3: ps_suppkey BIGINT Public), slice key: [#0: p_brand CHAR(10) Public, #1: p_type VARCHAR(25) Public, #2: p_size INTEGER Public, #3: ps_suppkey BIGINT Public]\n"
+ "            LogicalProject-Secure, schema:(#0: p_brand CHAR(10) Public,#1: p_type VARCHAR(25) Public,#2: p_size INTEGER Public,#3: ps_suppkey BIGINT Public)\n"
+ "                LogicalFilter-Secure, schema:(#0: ps_suppkey BIGINT Public,#1: p_brand CHAR(10) Public,#2: p_type VARCHAR(25) Public,#3: p_size INTEGER Public,#4: ps_suppkey0 BIGINT Public,#5: s_suppkey INTEGER Public,#6: $f1 BOOLEAN Public)\n"
+ "                    LogicalJoin-Secure, schema:(#0: ps_suppkey BIGINT Public,#1: p_brand CHAR(10) Public,#2: p_type VARCHAR(25) Public,#3: p_size INTEGER Public,#4: ps_suppkey0 BIGINT Public,#5: s_suppkey INTEGER Public,#6: $f1 BOOLEAN Public), slice key: [#4: ps_suppkey0 BIGINT Public, #5: s_suppkey INTEGER Public]\n"
+ "                        LogicalProject-Plain, schema:(#0: ps_suppkey BIGINT Public,#1: p_brand CHAR(10) Public,#2: p_type VARCHAR(25) Public,#3: p_size INTEGER Public,#4: ps_suppkey0 BIGINT Public)\n"
+ "                            LogicalFilter-Plain, schema:(#0: ps_partkey BIGINT Public,#1: ps_suppkey BIGINT Public,#2: p_partkey INTEGER Public,#3: p_brand CHAR(10) Public,#4: p_type VARCHAR(25) Public,#5: p_size INTEGER Public)\n"
+ "                                LogicalJoin-Plain, schema:(#0: ps_partkey BIGINT Public,#1: ps_suppkey BIGINT Public,#2: p_partkey INTEGER Public,#3: p_brand CHAR(10) Public,#4: p_type VARCHAR(25) Public,#5: p_size INTEGER Public)\n"
+ "                                    LogicalProject-Plain, schema:(#0: ps_partkey BIGINT Public,#1: ps_suppkey BIGINT Public)\n"
+ "                                        JdbcTableScan-Plain, schema:(#0: ps_partkey BIGINT Public,#1: ps_suppkey BIGINT Public,#2: ps_availqty INTEGER Private,#3: ps_supplycost DECIMAL(19, 0) Public,#4: ps_comment VARCHAR(199) Private)\n"
+ "                                    LogicalProject-Plain, schema:(#0: p_partkey INTEGER Public,#1: p_brand CHAR(10) Public,#2: p_type VARCHAR(25) Public,#3: p_size INTEGER Public)\n"
+ "                                        JdbcTableScan-Plain, schema:(#0: p_partkey INTEGER Public,#1: p_name VARCHAR(55) Public,#2: p_mfgr CHAR(25) Public,#3: p_brand CHAR(10) Public,#4: p_type VARCHAR(25) Public,#5: p_size INTEGER Public,#6: p_container CHAR(10) Public,#7: p_retailprice DECIMAL(19, 0) Public,#8: p_comment VARCHAR(23) Public)\n"
+ "                        LogicalAggregate-Secure, schema:(#0: s_suppkey INTEGER Public,#1: $f1 BOOLEAN Public), slice key: [#0: s_suppkey INTEGER Public]\n"
+ "                            LogicalProject-Plain, schema:(#0: s_suppkey INTEGER Public,#1: $f1 BOOLEAN Public), slice key: [#0: s_suppkey INTEGER Public]\n"
+ "                                LogicalFilter-Plain, schema:(#0: s_suppkey INTEGER Public,#1: s_comment VARCHAR(101) Private), slice key: [#0: s_suppkey INTEGER Public]\n"
+ "                                    LogicalProject-Plain, schema:(#0: s_suppkey INTEGER Public,#1: s_comment VARCHAR(101) Private)\n"
+ "                                        JdbcTableScan-Plain, schema:(#0: s_suppkey INTEGER Public,#1: s_name CHAR(25) Public,#2: s_address VARCHAR(40) Private,#3: s_nationkey BIGINT Public,#4: s_phone CHAR(15) Private,#5: s_acctbal DECIMAL(19, 0) Private,#6: s_comment VARCHAR(101) Private)\n"
,

          											     //  17
"LogicalProject-DistributedOblivious, schema:(#0: avg_yearly DECIMAL(19, 0) Private)\n"
+ "    LogicalAggregate-DistributedOblivious, schema:(#0: $f0 DECIMAL(19, 0) Private)\n"
+ "        LogicalProject-DistributedOblivious, schema:(#0: extendedprice DECIMAL(19, 0) Private)\n"
+ "            LogicalFilter-DistributedOblivious, schema:(#0: t_partkey INTEGER Private,#1: t_avg_quantity DECIMAL(19, 1) Private,#2: l_quantity DECIMAL(19, 0) Private,#3: l_partkey INTEGER Private,#4: l_extendedprice DECIMAL(19, 0) Private)\n"
+ "                LogicalJoin-DistributedOblivious, schema:(#0: t_partkey INTEGER Private,#1: t_avg_quantity DECIMAL(19, 1) Private,#2: l_quantity DECIMAL(19, 0) Private,#3: l_partkey INTEGER Private,#4: l_extendedprice DECIMAL(19, 0) Private), Predicate: =($3, $0)\n"
+ "                    LogicalProject-DistributedOblivious, schema:(#0: t_partkey INTEGER Private,#1: t_avg_quantity DECIMAL(19, 1) Private)\n"
+ "                        LogicalFilter-DistributedOblivious, schema:(#0: t_partkey INTEGER Private,#1: $f1 DECIMAL(19, 0) Private)\n"
+ "                            LogicalAggregate-DistributedOblivious, schema:(#0: t_partkey INTEGER Private,#1: $f1 DECIMAL(19, 0) Private)\n"
+ "                                LogicalProject-LocalClearPartitioned, schema:(#0: t_partkey INTEGER Private,#1: l_quantity DECIMAL(19, 0) Private)\n"
+ "                                    JdbcTableScan-LocalClearPartitioned, schema:(#0: l_orderkey INTEGER Private,#1: l_partkey INTEGER Private,#2: l_suppkey INTEGER Private,#3: l_linenumber INTEGER Private,#4: l_quantity DECIMAL(19, 0) Private,#5: l_extendedprice DECIMAL(19, 0) Private,#6: l_discount DECIMAL(19, 0) Private,#7: l_tax DECIMAL(19, 0) Private,#8: l_returnflag CHAR(1) Private,#9: l_linestatus CHAR(1) Private,#10: l_shipdate DATE Private,#11: l_commitdate DATE Private,#12: l_receiptdate DATE Private,#13: l_shipinstruct CHAR(25) Private,#14: l_shipmode CHAR(10) Private,#15: l_comment VARCHAR(44) Private)\n"
+ "                    LogicalProject-LocalObliviousPartitioned, schema:(#0: l_quantity DECIMAL(19, 0) Private,#1: l_partkey INTEGER Private,#2: l_extendedprice DECIMAL(19, 0) Private)\n"
+ "                        LogicalJoin-LocalObliviousPartitioned, schema:(#0: p_partkey INTEGER Public,#1: l_partkey INTEGER Private,#2: l_quantity DECIMAL(19, 0) Private,#3: l_extendedprice DECIMAL(19, 0) Private), Predicate: =($0, $1)\n"
+ "                            LogicalProject-LocalClearReplicated, schema:(#0: p_partkey INTEGER Public)\n"
+ "                                LogicalFilter-LocalClearReplicated, schema:(#0: p_partkey INTEGER Public,#1: p_brand CHAR(10) Public,#2: p_container CHAR(10) Public)\n"
+ "                                    LogicalProject-LocalClearReplicated, schema:(#0: p_partkey INTEGER Public,#1: p_brand CHAR(10) Public,#2: p_container CHAR(10) Public)\n"
+ "                                        JdbcTableScan-LocalClearReplicated, schema:(#0: p_partkey INTEGER Public,#1: p_name VARCHAR(55) Public,#2: p_mfgr CHAR(25) Public,#3: p_brand CHAR(10) Public,#4: p_type VARCHAR(25) Public,#5: p_size INTEGER Public,#6: p_container CHAR(10) Public,#7: p_retailprice DECIMAL(19, 0) Public,#8: p_comment VARCHAR(23) Public)\n"
+ "                            LogicalProject-LocalClearPartitioned, schema:(#0: l_partkey INTEGER Private,#1: l_quantity DECIMAL(19, 0) Private,#2: l_extendedprice DECIMAL(19, 0) Private)\n"
+ "                                JdbcTableScan-LocalClearPartitioned, schema:(#0: l_orderkey INTEGER Private,#1: l_partkey INTEGER Private,#2: l_suppkey INTEGER Private,#3: l_linenumber INTEGER Private,#4: l_quantity DECIMAL(19, 0) Private,#5: l_extendedprice DECIMAL(19, 0) Private,#6: l_discount DECIMAL(19, 0) Private,#7: l_tax DECIMAL(19, 0) Private,#8: l_returnflag CHAR(1) Private,#9: l_linestatus CHAR(1) Private,#10: l_shipdate DATE Private,#11: l_commitdate DATE Private,#12: l_receiptdate DATE Private,#13: l_shipinstruct CHAR(25) Private,#14: l_shipmode CHAR(10) Private,#15: l_comment VARCHAR(44) Private)\n",


          											     //  18
"LogicalSort-Secure, schema:(#0: c_name VARCHAR(25) Private,#1: c_custkey INTEGER Public,#2: o_orderkey INTEGER Public,#3: o_orderdate DATE Private,#4: o_totalprice DECIMAL(19, 0) Private,#5: EXPR$5 DECIMAL(19, 0) Private)\n"
+ "    LogicalAggregate-Secure, schema:(#0: c_name VARCHAR(25) Private,#1: c_custkey INTEGER Public,#2: o_orderkey INTEGER Public,#3: o_orderdate DATE Private,#4: o_totalprice DECIMAL(19, 0) Private,#5: EXPR$5 DECIMAL(19, 0) Private), slice key: [#1: c_custkey INTEGER Public, #2: o_orderkey INTEGER Public]\n"
+ "        LogicalProject-Secure, schema:(#0: c_name VARCHAR(25) Private,#1: c_custkey INTEGER Public,#2: o_orderkey INTEGER Public,#3: o_orderdate DATE Private,#4: o_totalprice DECIMAL(19, 0) Private,#5: l_quantity DECIMAL(19, 0) Private)\n"
+ "            LogicalJoin-Secure, schema:(#0: c_custkey INTEGER Public,#1: c_name VARCHAR(25) Private,#2: o_orderkey INTEGER Public,#3: o_custkey BIGINT Private,#4: o_totalprice DECIMAL(19, 0) Private,#5: o_orderdate DATE Private,#6: l_orderkey BIGINT Private,#7: l_quantity DECIMAL(19, 0) Private,#8: l_orderkey0 BIGINT Private)\n"
+ "                LogicalJoin-Secure, schema:(#0: c_custkey INTEGER Public,#1: c_name VARCHAR(25) Private,#2: o_orderkey INTEGER Public,#3: o_custkey BIGINT Private,#4: o_totalprice DECIMAL(19, 0) Private,#5: o_orderdate DATE Private,#6: l_orderkey BIGINT Private,#7: l_quantity DECIMAL(19, 0) Private)\n"
+ "                    LogicalJoin-Secure, schema:(#0: c_custkey INTEGER Public,#1: c_name VARCHAR(25) Private,#2: o_orderkey INTEGER Public,#3: o_custkey BIGINT Private,#4: o_totalprice DECIMAL(19, 0) Private,#5: o_orderdate DATE Private)\n"
+ "                        LogicalProject-Plain, schema:(#0: c_custkey INTEGER Public,#1: c_name VARCHAR(25) Private)\n"
+ "                            JdbcTableScan-Plain, schema:(#0: c_custkey INTEGER Public,#1: c_name VARCHAR(25) Private,#2: c_address VARCHAR(40) Private,#3: c_nationkey BIGINT Public,#4: c_phone CHAR(15) Private,#5: c_acctbal DECIMAL(19, 0) Private,#6: c_mktsegment CHAR(10) Private,#7: c_comment VARCHAR(117) Private)\n"
+ "                        LogicalProject-Plain, schema:(#0: o_orderkey INTEGER Public,#1: o_custkey BIGINT Private,#2: o_totalprice DECIMAL(19, 0) Private,#3: o_orderdate DATE Private)\n"
+ "                            JdbcTableScan-Plain, schema:(#0: o_orderkey INTEGER Public,#1: o_custkey BIGINT Private,#2: o_orderstatus CHAR(1) Private,#3: o_totalprice DECIMAL(19, 0) Private,#4: o_orderdate DATE Private,#5: o_orderpriority CHAR(15) Private,#6: o_clerk CHAR(15) Private,#7: o_shippriority INTEGER Private,#8: o_comment VARCHAR(79) Private)\n"
+ "                    LogicalProject-Plain, schema:(#0: l_orderkey BIGINT Private,#1: l_quantity DECIMAL(19, 0) Private)\n"
+ "                        JdbcTableScan-Plain, schema:(#0: l_orderkey BIGINT Private,#1: l_partkey BIGINT Private,#2: l_suppkey BIGINT Private,#3: l_linenumber INTEGER Private,#4: l_quantity DECIMAL(19, 0) Private,#5: l_extendedprice DECIMAL(19, 0) Private,#6: l_discount DECIMAL(19, 0) Private,#7: l_tax DECIMAL(19, 0) Private,#8: l_returnflag CHAR(1) Private,#9: l_linestatus CHAR(1) Private,#10: l_shipdate DATE Private,#11: l_commitdate DATE Private,#12: l_receiptdate DATE Private,#13: l_shipinstruct CHAR(25) Private,#14: l_shipmode CHAR(10) Private,#15: l_comment VARCHAR(44) Private)\n"
+ "                LogicalProject-Secure, schema:(#0: l_orderkey BIGINT Private)\n"
+ "                    LogicalFilter-Secure, schema:(#0: l_orderkey BIGINT Private,#1: $f1 DECIMAL(19, 0) Private)\n"
+ "                        LogicalAggregate-Secure, schema:(#0: l_orderkey BIGINT Private,#1: $f1 DECIMAL(19, 0) Private)\n"
+ "                            LogicalProject-Plain, schema:(#0: l_orderkey BIGINT Private,#1: l_quantity DECIMAL(19, 0) Private)\n"
+ "                                JdbcTableScan-Plain, schema:(#0: l_orderkey BIGINT Private,#1: l_partkey BIGINT Private,#2: l_suppkey BIGINT Private,#3: l_linenumber INTEGER Private,#4: l_quantity DECIMAL(19, 0) Private,#5: l_extendedprice DECIMAL(19, 0) Private,#6: l_discount DECIMAL(19, 0) Private,#7: l_tax DECIMAL(19, 0) Private,#8: l_returnflag CHAR(1) Private,#9: l_linestatus CHAR(1) Private,#10: l_shipdate DATE Private,#11: l_commitdate DATE Private,#12: l_receiptdate DATE Private,#13: l_shipinstruct CHAR(25) Private,#14: l_shipmode CHAR(10) Private,#15: l_comment VARCHAR(44) Private)\n"
,

          											     //  19
"LogicalAggregate-Secure, schema:(#0: revenue DECIMAL(19, 0) Private)\n"
+ "    LogicalProject-Secure, schema:(#0: $f0 DECIMAL(19, 0) Private)\n"
+ "        LogicalJoin-Secure, schema:(#0: l_partkey BIGINT Private,#1: l_quantity DECIMAL(19, 0) Private,#2: l_extendedprice DECIMAL(19, 0) Private,#3: l_discount DECIMAL(19, 0) Private,#4: l_shipinstruct CHAR(25) Private,#5: l_shipmode CHAR(10) Private,#6: p_partkey INTEGER Public,#7: p_brand CHAR(10) Public,#8: p_size INTEGER Public,#9: p_container CHAR(10) Public)\n"
+ "            LogicalProject-Plain, schema:(#0: l_partkey BIGINT Private,#1: l_quantity DECIMAL(19, 0) Private,#2: l_extendedprice DECIMAL(19, 0) Private,#3: l_discount DECIMAL(19, 0) Private,#4: l_shipinstruct CHAR(25) Private,#5: l_shipmode CHAR(10) Private)\n"
+ "                JdbcTableScan-Plain, schema:(#0: l_orderkey BIGINT Private,#1: l_partkey BIGINT Private,#2: l_suppkey BIGINT Private,#3: l_linenumber INTEGER Private,#4: l_quantity DECIMAL(19, 0) Private,#5: l_extendedprice DECIMAL(19, 0) Private,#6: l_discount DECIMAL(19, 0) Private,#7: l_tax DECIMAL(19, 0) Private,#8: l_returnflag CHAR(1) Private,#9: l_linestatus CHAR(1) Private,#10: l_shipdate DATE Private,#11: l_commitdate DATE Private,#12: l_receiptdate DATE Private,#13: l_shipinstruct CHAR(25) Private,#14: l_shipmode CHAR(10) Private,#15: l_comment VARCHAR(44) Private)\n"
+ "            LogicalProject-Plain, schema:(#0: p_partkey INTEGER Public,#1: p_brand CHAR(10) Public,#2: p_size INTEGER Public,#3: p_container CHAR(10) Public)\n"
+ "                JdbcTableScan-Plain, schema:(#0: p_partkey INTEGER Public,#1: p_name VARCHAR(55) Public,#2: p_mfgr CHAR(25) Public,#3: p_brand CHAR(10) Public,#4: p_type VARCHAR(25) Public,#5: p_size INTEGER Public,#6: p_container CHAR(10) Public,#7: p_retailprice DECIMAL(19, 0) Public,#8: p_comment VARCHAR(23) Public)\n"
,

          											     //  20
"LogicalProject-Secure, schema:(#0: s_name CHAR(25) Public,#1: s_address VARCHAR(40) Private)\n"
+ "    LogicalSort-Secure, schema:(#0: s_name CHAR(25) Public,#1: s_address VARCHAR(40) Private,#2: s_nationkey BIGINT Public,#3: n_nationkey INTEGER Public)\n"
+ "        LogicalJoin-Plain, schema:(#0: s_name CHAR(25) Public,#1: s_address VARCHAR(40) Private,#2: s_nationkey BIGINT Public,#3: n_nationkey INTEGER Public)\n"
+ "            LogicalProject-Secure, schema:(#0: s_name CHAR(25) Public,#1: s_address VARCHAR(40) Private,#2: s_nationkey BIGINT Public)\n"
+ "                LogicalFilter-Secure, schema:(#0: s_name CHAR(25) Public,#1: s_address VARCHAR(40) Private,#2: s_nationkey BIGINT Public,#3: s_suppkey0 BIGINT Public,#4: ps_suppkey BIGINT Public)\n"
+ "                    LogicalJoin-Plain, schema:(#0: s_name CHAR(25) Public,#1: s_address VARCHAR(40) Private,#2: s_nationkey BIGINT Public,#3: s_suppkey0 BIGINT Public,#4: ps_suppkey BIGINT Public), slice key: [#3: s_suppkey0 BIGINT Public, #4: ps_suppkey BIGINT Public]\n"
+ "                        LogicalProject-Plain, schema:(#0: s_name CHAR(25) Public,#1: s_address VARCHAR(40) Private,#2: s_nationkey BIGINT Public,#3: s_suppkey0 BIGINT Public)\n"
+ "                            JdbcTableScan-Plain, schema:(#0: s_suppkey INTEGER Public,#1: s_name CHAR(25) Public,#2: s_address VARCHAR(40) Private,#3: s_nationkey BIGINT Public,#4: s_phone CHAR(15) Private,#5: s_acctbal DECIMAL(19, 0) Private,#6: s_comment VARCHAR(101) Private)\n"
+ "                        LogicalAggregate-Secure, schema:(#0: ps_suppkey BIGINT Public), slice key: [#0: ps_suppkey BIGINT Public]\n"
+ "                            LogicalProject-Secure, schema:(#0: ps_suppkey BIGINT Public)\n"
+ "                                LogicalFilter-Secure, schema:(#0: l_partkey BIGINT Private,#1: l_suppkey BIGINT Private,#2: inventory DECIMAL(19, 1) Private,#3: ps_partkey BIGINT Public,#4: ps_suppkey BIGINT Public,#5: ps_availqty INTEGER Private,#6: p_partkey INTEGER Public,#7: p_partkey0 BIGINT Public)\n"
+ "                                    LogicalJoin-Plain, schema:(#0: l_partkey BIGINT Private,#1: l_suppkey BIGINT Private,#2: inventory DECIMAL(19, 1) Private,#3: ps_partkey BIGINT Public,#4: ps_suppkey BIGINT Public,#5: ps_availqty INTEGER Private,#6: p_partkey INTEGER Public,#7: p_partkey0 BIGINT Public), slice key: [#3: ps_partkey BIGINT Public, #7: p_partkey0 BIGINT Public]\n"
+ "                                        LogicalJoin-Secure, schema:(#0: l_partkey BIGINT Private,#1: l_suppkey BIGINT Private,#2: inventory DECIMAL(19, 1) Private,#3: ps_partkey BIGINT Public,#4: ps_suppkey BIGINT Public,#5: ps_availqty INTEGER Private)\n"
+ "                                            LogicalProject-Secure, schema:(#0: l_partkey BIGINT Private,#1: l_suppkey BIGINT Private,#2: inventory DECIMAL(19, 1) Private)\n"
+ "                                                LogicalAggregate-Secure, schema:(#0: l_partkey BIGINT Private,#1: l_suppkey BIGINT Private,#2: $f2 DECIMAL(19, 0) Private)\n"
+ "                                                    LogicalProject-Plain, schema:(#0: l_partkey BIGINT Private,#1: l_suppkey BIGINT Private,#2: l_quantity DECIMAL(19, 0) Private)\n"
+ "                                                        LogicalFilter-Plain, schema:(#0: l_partkey BIGINT Private,#1: l_suppkey BIGINT Private,#2: l_quantity DECIMAL(19, 0) Private,#3: l_shipdate DATE Private)\n"
+ "                                                            LogicalProject-Plain, schema:(#0: l_partkey BIGINT Private,#1: l_suppkey BIGINT Private,#2: l_quantity DECIMAL(19, 0) Private,#3: l_shipdate DATE Private)\n"
+ "                                                                JdbcTableScan-Plain, schema:(#0: l_orderkey BIGINT Private,#1: l_partkey BIGINT Private,#2: l_suppkey BIGINT Private,#3: l_linenumber INTEGER Private,#4: l_quantity DECIMAL(19, 0) Private,#5: l_extendedprice DECIMAL(19, 0) Private,#6: l_discount DECIMAL(19, 0) Private,#7: l_tax DECIMAL(19, 0) Private,#8: l_returnflag CHAR(1) Private,#9: l_linestatus CHAR(1) Private,#10: l_shipdate DATE Private,#11: l_commitdate DATE Private,#12: l_receiptdate DATE Private,#13: l_shipinstruct CHAR(25) Private,#14: l_shipmode CHAR(10) Private,#15: l_comment VARCHAR(44) Private)\n"
+ "                                            LogicalProject-Plain, schema:(#0: ps_partkey BIGINT Public,#1: ps_suppkey BIGINT Public,#2: ps_availqty INTEGER Private)\n"
+ "                                                JdbcTableScan-Plain, schema:(#0: ps_partkey BIGINT Public,#1: ps_suppkey BIGINT Public,#2: ps_availqty INTEGER Private,#3: ps_supplycost DECIMAL(19, 0) Public,#4: ps_comment VARCHAR(199) Private)\n"
+ "                                        LogicalProject-Plain, schema:(#0: p_partkey INTEGER Public,#1: p_partkey0 BIGINT Public)\n"
+ "                                            LogicalAggregate-Plain, schema:(#0: p_partkey INTEGER Public)\n"
+ "                                                LogicalProject-Plain, schema:(#0: p_partkey INTEGER Public)\n"
+ "                                                    LogicalFilter-Plain, schema:(#0: p_partkey INTEGER Public,#1: p_name VARCHAR(55) Public)\n"
+ "                                                        LogicalProject-Plain, schema:(#0: p_partkey INTEGER Public,#1: p_name VARCHAR(55) Public)\n"
+ "                                                            JdbcTableScan-Plain, schema:(#0: p_partkey INTEGER Public,#1: p_name VARCHAR(55) Public,#2: p_mfgr CHAR(25) Public,#3: p_brand CHAR(10) Public,#4: p_type VARCHAR(25) Public,#5: p_size INTEGER Public,#6: p_container CHAR(10) Public,#7: p_retailprice DECIMAL(19, 0) Public,#8: p_comment VARCHAR(23) Public)\n"
+ "            LogicalProject-Plain, schema:(#0: n_nationkey INTEGER Public)\n"
+ "                LogicalFilter-Plain, schema:(#0: n_nationkey INTEGER Public,#1: n_name CHAR(25) Public)\n"
+ "                    LogicalProject-Plain, schema:(#0: n_nationkey INTEGER Public,#1: n_name CHAR(25) Public)\n"
+ "                        JdbcTableScan-Plain, schema:(#0: n_nationkey INTEGER Public,#1: n_name CHAR(25) Public,#2: n_regionkey BIGINT Public,#3: n_comment VARCHAR(152) Public)\n"
,

          											     //  21
"LogicalSort-Secure, schema:(#0: s_name CHAR(25) Public,#1: numwait BIGINT Public)\n"
+ "    LogicalAggregate-Secure, schema:(#0: s_name CHAR(25) Public,#1: numwait BIGINT Public), slice key: [#0: s_name CHAR(25) Public]\n"
+ "        LogicalProject-Secure, schema:(#0: s_name CHAR(25) Public)\n"
+ "            LogicalJoin-Secure, schema:(#0: s_name CHAR(25) Public,#1: l_orderkey BIGINT Private,#2: l_suppkey BIGINT Private,#3: l_suppkey0 BIGINT Private,#4: l_orderkey0 BIGINT Private)\n"
+ "                LogicalProject-Secure, schema:(#0: s_name CHAR(25) Public,#1: l_orderkey BIGINT Private,#2: l_suppkey BIGINT Private)\n"
+ "                    LogicalSort-Secure, schema:(#0: s_suppkey INTEGER Public,#1: s_name CHAR(25) Public,#2: s_nationkey BIGINT Public,#3: l_suppkey BIGINT Private,#4: l_orderkey BIGINT Private,#5: o_orderkey INTEGER Public,#6: n_nationkey INTEGER Public)\n"
+ "                        LogicalJoin-Plain, schema:(#0: s_suppkey INTEGER Public,#1: s_name CHAR(25) Public,#2: s_nationkey BIGINT Public,#3: l_suppkey BIGINT Private,#4: l_orderkey BIGINT Private,#5: o_orderkey INTEGER Public,#6: n_nationkey INTEGER Public)\n"
+ "                            LogicalJoin-Secure, schema:(#0: s_suppkey INTEGER Public,#1: s_name CHAR(25) Public,#2: s_nationkey BIGINT Public,#3: l_suppkey BIGINT Private,#4: l_orderkey BIGINT Private,#5: o_orderkey INTEGER Public)\n"
+ "                                LogicalJoin-Secure, schema:(#0: s_suppkey INTEGER Public,#1: s_name CHAR(25) Public,#2: s_nationkey BIGINT Public,#3: l_suppkey BIGINT Private,#4: l_orderkey BIGINT Private)\n"
+ "                                    LogicalProject-Plain, schema:(#0: s_suppkey INTEGER Public,#1: s_name CHAR(25) Public,#2: s_nationkey BIGINT Public)\n"
+ "                                        JdbcTableScan-Plain, schema:(#0: s_suppkey INTEGER Public,#1: s_name CHAR(25) Public,#2: s_address VARCHAR(40) Private,#3: s_nationkey BIGINT Public,#4: s_phone CHAR(15) Private,#5: s_acctbal DECIMAL(19, 0) Private,#6: s_comment VARCHAR(101) Private)\n"
+ "                                    LogicalProject-Secure, schema:(#0: l_suppkey BIGINT Private,#1: l_orderkey BIGINT Private)\n"
+ "                                        LogicalFilter-Secure, schema:(#0: l_orderkey BIGINT Private,#1: l_suppkey BIGINT Private,#2: l_commitdate DATE Private,#3: l_receiptdate DATE Private,#4: l_orderkey0 BIGINT Private,#5: l_suppkey0 BIGINT Private)\n"
+ "                                            LogicalFilter-Secure, schema:(#0: l_orderkey BIGINT Private,#1: l_suppkey BIGINT Private,#2: l_commitdate DATE Private,#3: l_receiptdate DATE Private,#4: l_orderkey0 BIGINT Private,#5: l_suppkey0 BIGINT Private)\n"
+ "                                                LogicalJoin-Secure, schema:(#0: l_orderkey BIGINT Private,#1: l_suppkey BIGINT Private,#2: l_commitdate DATE Private,#3: l_receiptdate DATE Private,#4: l_orderkey0 BIGINT Private,#5: l_suppkey0 BIGINT Private)\n"
+ "                                                    LogicalProject-Plain, schema:(#0: l_orderkey BIGINT Private,#1: l_suppkey BIGINT Private,#2: l_commitdate DATE Private,#3: l_receiptdate DATE Private)\n"
+ "                                                        JdbcTableScan-Plain, schema:(#0: l_orderkey BIGINT Private,#1: l_partkey BIGINT Private,#2: l_suppkey BIGINT Private,#3: l_linenumber INTEGER Private,#4: l_quantity DECIMAL(19, 0) Private,#5: l_extendedprice DECIMAL(19, 0) Private,#6: l_discount DECIMAL(19, 0) Private,#7: l_tax DECIMAL(19, 0) Private,#8: l_returnflag CHAR(1) Private,#9: l_linestatus CHAR(1) Private,#10: l_shipdate DATE Private,#11: l_commitdate DATE Private,#12: l_receiptdate DATE Private,#13: l_shipinstruct CHAR(25) Private,#14: l_shipmode CHAR(10) Private,#15: l_comment VARCHAR(44) Private)\n"
+ "                                                    LogicalAggregate-Secure, schema:(#0: l_orderkey BIGINT Private,#1: l_suppkey BIGINT Private)\n"
+ "                                                        LogicalProject-Plain, schema:(#0: l_orderkey BIGINT Private,#1: l_suppkey BIGINT Private)\n"
+ "                                                            LogicalFilter-Plain, schema:(#0: l_orderkey BIGINT Private,#1: l_suppkey BIGINT Private,#2: l_commitdate DATE Private,#3: l_receiptdate DATE Private)\n"
+ "                                                                LogicalProject-Plain, schema:(#0: l_orderkey BIGINT Private,#1: l_suppkey BIGINT Private,#2: l_commitdate DATE Private,#3: l_receiptdate DATE Private)\n"
+ "                                                                    JdbcTableScan-Plain, schema:(#0: l_orderkey BIGINT Private,#1: l_partkey BIGINT Private,#2: l_suppkey BIGINT Private,#3: l_linenumber INTEGER Private,#4: l_quantity DECIMAL(19, 0) Private,#5: l_extendedprice DECIMAL(19, 0) Private,#6: l_discount DECIMAL(19, 0) Private,#7: l_tax DECIMAL(19, 0) Private,#8: l_returnflag CHAR(1) Private,#9: l_linestatus CHAR(1) Private,#10: l_shipdate DATE Private,#11: l_commitdate DATE Private,#12: l_receiptdate DATE Private,#13: l_shipinstruct CHAR(25) Private,#14: l_shipmode CHAR(10) Private,#15: l_comment VARCHAR(44) Private)\n"
+ "                                LogicalProject-Plain, schema:(#0: o_orderkey INTEGER Public)\n"
+ "                                    LogicalFilter-Plain, schema:(#0: o_orderkey INTEGER Public,#1: o_orderstatus CHAR(1) Private)\n"
+ "                                        LogicalProject-Plain, schema:(#0: o_orderkey INTEGER Public,#1: o_orderstatus CHAR(1) Private)\n"
+ "                                            JdbcTableScan-Plain, schema:(#0: o_orderkey INTEGER Public,#1: o_custkey BIGINT Private,#2: o_orderstatus CHAR(1) Private,#3: o_totalprice DECIMAL(19, 0) Private,#4: o_orderdate DATE Private,#5: o_orderpriority CHAR(15) Private,#6: o_clerk CHAR(15) Private,#7: o_shippriority INTEGER Private,#8: o_comment VARCHAR(79) Private)\n"
+ "                            LogicalProject-Plain, schema:(#0: n_nationkey INTEGER Public)\n"
+ "                                LogicalFilter-Plain, schema:(#0: n_nationkey INTEGER Public,#1: n_name CHAR(25) Public)\n"
+ "                                    LogicalProject-Plain, schema:(#0: n_nationkey INTEGER Public,#1: n_name CHAR(25) Public)\n"
+ "                                        JdbcTableScan-Plain, schema:(#0: n_nationkey INTEGER Public,#1: n_name CHAR(25) Public,#2: n_regionkey BIGINT Public,#3: n_comment VARCHAR(152) Public)\n"
+ "                LogicalFilter-Secure, schema:(#0: l_suppkey BIGINT Private,#1: l_orderkey BIGINT Private)\n"
+ "                    LogicalAggregate-Secure, schema:(#0: l_suppkey BIGINT Private,#1: l_orderkey BIGINT Private)\n"
+ "                        LogicalProject-Secure, schema:(#0: l_suppkey BIGINT Private,#1: l_orderkey BIGINT Private)\n"
+ "                            LogicalJoin-Secure, schema:(#0: l_orderkey BIGINT Private,#1: l_suppkey BIGINT Private,#2: l_suppkey0 BIGINT Private,#3: l_orderkey0 BIGINT Private)\n"
+ "                                LogicalProject-Plain, schema:(#0: l_orderkey BIGINT Private,#1: l_suppkey BIGINT Private)\n"
+ "                                    JdbcTableScan-Plain, schema:(#0: l_orderkey BIGINT Private,#1: l_partkey BIGINT Private,#2: l_suppkey BIGINT Private,#3: l_linenumber INTEGER Private,#4: l_quantity DECIMAL(19, 0) Private,#5: l_extendedprice DECIMAL(19, 0) Private,#6: l_discount DECIMAL(19, 0) Private,#7: l_tax DECIMAL(19, 0) Private,#8: l_returnflag CHAR(1) Private,#9: l_linestatus CHAR(1) Private,#10: l_shipdate DATE Private,#11: l_commitdate DATE Private,#12: l_receiptdate DATE Private,#13: l_shipinstruct CHAR(25) Private,#14: l_shipmode CHAR(10) Private,#15: l_comment VARCHAR(44) Private)\n"
+ "                                LogicalAggregate-Secure, schema:(#0: l_suppkey BIGINT Private,#1: l_orderkey BIGINT Private)\n"
+ "                                    LogicalProject-Plain, schema:(#0: l_suppkey BIGINT Private,#1: l_orderkey BIGINT Private)\n"
+ "                                        JdbcTableScan-Plain, schema:(#0: l_orderkey BIGINT Private,#1: l_partkey BIGINT Private,#2: l_suppkey BIGINT Private,#3: l_linenumber INTEGER Private,#4: l_quantity DECIMAL(19, 0) Private,#5: l_extendedprice DECIMAL(19, 0) Private,#6: l_discount DECIMAL(19, 0) Private,#7: l_tax DECIMAL(19, 0) Private,#8: l_returnflag CHAR(1) Private,#9: l_linestatus CHAR(1) Private,#10: l_shipdate DATE Private,#11: l_commitdate DATE Private,#12: l_receiptdate DATE Private,#13: l_shipinstruct CHAR(25) Private,#14: l_shipmode CHAR(10) Private,#15: l_comment VARCHAR(44) Private)\n"
,

          											     //  22
"LogicalSort-Secure, schema:(#0: cntrycode VARCHAR(15) Private,#1: numcust BIGINT Private,#2: totacctbal DECIMAL(19, 0) Private)\n"
+ "    LogicalAggregate-Secure, schema:(#0: cntrycode VARCHAR(15) Private,#1: numcust BIGINT Private,#2: totacctbal DECIMAL(19, 0) Private)\n"
+ "        LogicalProject-Secure, schema:(#0: cntrycode VARCHAR(15) Private,#1: c_acctbal DECIMAL(19, 0) Private)\n"
+ "            LogicalJoin-Secure, schema:(#0: cntrycode VARCHAR(15) Private,#1: c_acctbal DECIMAL(19, 0) Private,#2: $f0 DECIMAL(19, 0) Private)\n"
+ "                LogicalProject-Secure, schema:(#0: cntrycode VARCHAR(15) Private,#1: c_acctbal DECIMAL(19, 0) Private)\n"
+ "                    LogicalFilter-Secure, schema:(#0: cntrycode VARCHAR(15) Private,#1: c_acctbal DECIMAL(19, 0) Private,#2: c_custkey0 BIGINT Public,#3: o_custkey BIGINT Private,#4: o_clerk CHAR(15) Private)\n"
+ "                        LogicalJoin-Secure, schema:(#0: cntrycode VARCHAR(15) Private,#1: c_acctbal DECIMAL(19, 0) Private,#2: c_custkey0 BIGINT Public,#3: o_custkey BIGINT Private,#4: o_clerk CHAR(15) Private)\n"
+ "                            LogicalProject-Plain, schema:(#0: cntrycode VARCHAR(15) Private,#1: c_acctbal DECIMAL(19, 0) Private,#2: c_custkey0 BIGINT Public)\n"
+ "                                LogicalFilter-Plain, schema:(#0: c_custkey INTEGER Public,#1: c_phone CHAR(15) Private,#2: c_acctbal DECIMAL(19, 0) Private)\n"
+ "                                    LogicalProject-Plain, schema:(#0: c_custkey INTEGER Public,#1: c_phone CHAR(15) Private,#2: c_acctbal DECIMAL(19, 0) Private)\n"
+ "                                        JdbcTableScan-Plain, schema:(#0: c_custkey INTEGER Public,#1: c_name VARCHAR(25) Private,#2: c_address VARCHAR(40) Private,#3: c_nationkey BIGINT Public,#4: c_phone CHAR(15) Private,#5: c_acctbal DECIMAL(19, 0) Private,#6: c_mktsegment CHAR(10) Private,#7: c_comment VARCHAR(117) Private)\n"
+ "                            LogicalProject-Plain, schema:(#0: o_custkey BIGINT Private,#1: o_clerk CHAR(15) Private)\n"
+ "                                JdbcTableScan-Plain, schema:(#0: o_orderkey INTEGER Public,#1: o_custkey BIGINT Private,#2: o_orderstatus CHAR(1) Private,#3: o_totalprice DECIMAL(19, 0) Private,#4: o_orderdate DATE Private,#5: o_orderpriority CHAR(15) Private,#6: o_clerk CHAR(15) Private,#7: o_shippriority INTEGER Private,#8: o_comment VARCHAR(79) Private)\n"
+ "                LogicalAggregate-Secure, schema:(#0: $f0 DECIMAL(19, 0) Private)\n"
+ "                    LogicalAggregate-Secure, schema:(#0: EXPR$0 DECIMAL(19, 0) Private)\n"
+ "                        LogicalProject-Plain, schema:(#0: c_acctbal DECIMAL(19, 0) Private)\n"
+ "                            LogicalFilter-Plain, schema:(#0: c_phone CHAR(15) Private,#1: c_acctbal DECIMAL(19, 0) Private)\n"
+ "                                LogicalFilter-Plain, schema:(#0: c_phone CHAR(15) Private,#1: c_acctbal DECIMAL(19, 0) Private)\n"
+ "                                    LogicalProject-Plain, schema:(#0: c_phone CHAR(15) Private,#1: c_acctbal DECIMAL(19, 0) Private)\n"
+ "                                        JdbcTableScan-Plain, schema:(#0: c_custkey INTEGER Public,#1: c_name VARCHAR(25) Private,#2: c_address VARCHAR(40) Private,#3: c_nationkey BIGINT Public,#4: c_phone CHAR(15) Private,#5: c_acctbal DECIMAL(19, 0) Private,#6: c_mktsegment CHAR(10) Private,#7: c_comment VARCHAR(117) Private)\n"







				);
      public void testQuery01() throws Exception {
          String expectedOutput =  PLANS.get(0);

          runTest(1, expectedOutput);
     }

  /*    public void testQuery02() throws Exception {
          String expectedOutput = PLANS.get(1);
          
          runTest(2, expectedOutput);
     }*/

      public void testQuery03() throws Exception {
          String expectedOutput = PLANS.get(2);
          runTest(3, expectedOutput);
     }

      public void testQuery04() throws Exception {
          String expectedOutput = PLANS.get(3);
          runTest(4, expectedOutput);
     }

      public void testQuery05() throws Exception {
    	  // TODO: push down access to replicated tables, make s_nationkey join on replicated nation key rather than doing it in MPC
          String expectedOutput = PLANS.get(4);
          runTest(5, expectedOutput);
     } /*
*/
      public void testQuery06() throws Exception {
          String expectedOutput = PLANS.get(5);
          runTest(6, expectedOutput);
      }

/*      public void testQuery07() throws Exception {
          String expectedOutput = PLANS.get(6);
          runTest(7, expectedOutput);
      }

      public void testQuery08() throws Exception {
    	  // expected to fail because need to push down public joins
          String expectedOutput = PLANS.get(7);
          runTest(8, expectedOutput);
      }

      public void testQuery09() throws Exception {
    	  // expected to fail because need to push down public join
    	  String expectedOutput = PLANS.get(8);
          runTest(9, expectedOutput);
      }

      public void testQuery10() throws Exception {
    	  // expected to fail because need to push down public join
    	  String expectedOutput = PLANS.get(9);
          runTest(10, expectedOutput);
      }
*/
      public void testQuery11() throws Exception {
    	  
    	  // TODO: push down join with nation 2X
          String expectedOutput = PLANS.get(10);
          runTest(11, expectedOutput);
      } 
      
      

      public void testQuery12() throws Exception {
          String expectedOutput = PLANS.get(11);
          runTest(12, expectedOutput);
      } 

      public void testQuery13() throws Exception {
          String expectedOutput = PLANS.get(12);
          runTest(13, expectedOutput);
      }

      public void testQuery14() throws Exception {
    	  
    	  // TODO: this is another where the join can be done in the clear owing to partitioning
          String expectedOutput = PLANS.get(13);
          runTest(14, expectedOutput);
         
      }

/*      public void testQuery15() throws Exception {
          String expectedOutput = PLANS.get(14);
          runTest(15, expectedOutput);
      }
*/
      public void testQuery16() throws Exception {
    	  // aggregate on supplier is oblivious b/c of secret  filter
    	  // TODO: examine design point of running local oblivious query in plaintext w/o padding
    	  // compare to query rewrite
    	  
    	  // TODO: fix this to make sure that attrs are being labelled correctly as they pass up the tree
    	  // if an op has an oblivious child, then it becomes private going forward
    	  // can look at src policy for partition-by decisions
          String expectedOutput = PLANS.get(15);
          runTest(16, expectedOutput);
      }

      public void testQuery17() throws Exception {
          String expectedOutput = PLANS.get(16);
          runTest(17, expectedOutput);
      }
/*
      public void testQuery18() throws Exception {
          String expectedOutput = PLANS.get(17);
          runTest(18, expectedOutput);
      }

      public void testQuery19() throws Exception {
          String expectedOutput = PLANS.get(18);
          runTest(19, expectedOutput);
      }

      public void testQuery20() throws Exception {
          String expectedOutput = PLANS.get(19);
          runTest(20, expectedOutput);
      }

      public void testQuery21() throws Exception {
          String expectedOutput = PLANS.get(20);
          runTest(21, expectedOutput);
      }

      public void testQuery22() throws Exception {
          String expectedOutput = PLANS.get(21);
          runTest(22, expectedOutput);
      }
*/
	  
	  void runTest(int queryNo, String expectedTree) throws Exception {

			String sql = super.readSQL(queryNo);
			String testName = "q" + queryNo;
			
			logger.log(Level.INFO, "Parsing " + sql);
			SecureRelRoot secRoot = new SecureRelRoot(testName, sql);
		
			logger.log(Level.INFO, "Parsed " + RelOptUtil.toString(secRoot.getRelRoot().project()));
			
			String testTree = secRoot.toString();
			logger.log(Level.INFO, "Resolved tree to:\n " + testTree);
			assertEquals(expectedTree, testTree);
		}
}
