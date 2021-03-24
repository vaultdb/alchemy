#!/bin/bash
#run from src/main/cpp

psql  -t --csv  -c "SELECT * FROM lineitem ORDER BY (1), (2)  LIMIT 50" tpch_unioned  > test/support/csv/lineitem.csv
psql  -t --csv  -c "SELECT * FROM orders ORDER BY (1), (2)  LIMIT 50" tpch_unioned  > test/support/csv/orders.csv
psql  -t --csv  -c "SELECT * FROM customer ORDER BY (1), (2)  LIMIT 50" tpch_unioned  > test/support/csv/customer.csv
