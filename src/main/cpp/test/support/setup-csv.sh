#!/bin/bash
#run from src/main/cpp

#psql  -t --csv  -c "SELECT l_orderkey,  l_partkey,  l_suppkey,  l_linenumber,  l_quantity,  l_extendedprice,  l_discount,  l_tax,  l_returnflag,  l_linestatus,  CAST(EXTRACT(EPOCH FROM l_shipdate) AS BIGINT), CAST(EXTRACT(EPOCH FROM l_commitdate) AS BIGINT), CAST(EXTRACT(EPOCH FROM l_receiptdate) AS BIGINT),  l_shipinstruct,  l_shipmode,  l_comment FROM lineitem ORDER BY (1), (2)  LIMIT 50" tpch_unioned  > test/support/csv/lineitem.csv
psql  -t --csv  -c "SELECT  l_orderkey, l_partkey, l_suppkey, l_linenumber, l_quantity, l_extendedprice, l_discount, l_tax, l_returnflag, l_linestatus, l_shipdate, l_commitdate, l_receiptdate, l_shipinstruct, l_shipmode, l_comment FROM lineitem ORDER BY (1), (2)  LIMIT 50" tpch_unioned_150  > test/support/csv/lineitem.csv
psql  -t --csv  -c "SELECT o_orderkey, o_custkey, o_orderstatus, o_totalprice, o_orderdate, o_orderpriority, o_comment FROM orders ORDER BY (1), (2)  LIMIT 50" tpch_unioned_150  > test/support/csv/orders.csv
psql  -t --csv  -c "SELECT c_custkey, c_name, c_address, c_nationkey, c_phone, c_acctbal, c_mktsegment, c_comment FROM customer ORDER BY (1), (2)  LIMIT 50" tpch_unioned_150  > test/support/csv/customer.csv



