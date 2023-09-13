#!/bin/bash

psql tpch_unioned_150 -c "DROP TABLE IF EXISTS order_keys"
psql tpch_unioned_150 -c "SELECT o_orderkey INTO order_keys FROM orders"
echo "DROP TABLE IF EXISTS order_keys;" > order_keys.sql
pg_dump tpch_unioned_150 -t order_keys >> order_keys.sql
psql tpch_alice_150 < order_keys.sql
psql tpch_bob_150 < order_keys.sql

psql tpch_unioned_300 -c "DROP TABLE IF EXISTS order_keys"
psql tpch_unioned_300 -c "SELECT o_orderkey INTO order_keys FROM orders"
echo "DROP TABLE IF EXISTS order_keys;" > order_keys.sql
pg_dump tpch_unioned_300 -t order_keys >> order_keys.sql
psql tpch_alice_300 < order_keys.sql
psql tpch_bob_300 < order_keys.sql

psql tpch_unioned_600 -c "DROP TABLE IF EXISTS order_keys"
psql tpch_unioned_600 -c "SELECT o_orderkey INTO order_keys FROM orders"
echo "DROP TABLE IF EXISTS order_keys;" > order_keys.sql
pg_dump tpch_unioned_600 -t order_keys >> order_keys.sql
psql tpch_alice_600 < order_keys.sql
psql tpch_bob_600 < order_keys.sql

psql tpch_unioned_900 -c "DROP TABLE IF EXISTS order_keys"
psql tpch_unioned_900 -c "SELECT o_orderkey INTO order_keys FROM orders"
echo "DROP TABLE IF EXISTS order_keys;" > order_keys.sql
pg_dump tpch_unioned_900 -t order_keys >> order_keys.sql
psql tpch_alice_900 < order_keys.sql
psql tpch_bob_900 < order_keys.sql

psql tpch_unioned_1200 -c "DROP TABLE IF EXISTS order_keys"
psql tpch_unioned_1200 -c "SELECT o_orderkey INTO order_keys FROM orders"
echo "DROP TABLE IF EXISTS order_keys;" > order_keys.sql
pg_dump tpch_unioned_1200 -t order_keys >> order_keys.sql
psql tpch_alice_1200 < order_keys.sql
psql tpch_bob_1200 < order_keys.sql

psql tpch_unioned_2400 -c "DROP TABLE IF EXISTS order_keys"
psql tpch_unioned_2400 -c "SELECT o_orderkey INTO order_keys FROM orders"
echo "DROP TABLE IF EXISTS order_keys;" > order_keys.sql
pg_dump tpch_unioned_2400 -t order_keys >> order_keys.sql
psql tpch_alice_2400 < order_keys.sql
psql tpch_bob_2400 < order_keys.sql
