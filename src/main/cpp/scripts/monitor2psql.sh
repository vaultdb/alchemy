#!/bin/bash

# usage: bash scripts/monitor2psql.sh <test name> <psql db name>
# e.g.
# bash scripts/monitor2psql.sh secure_cloud_setting_test vaultdb_monitoring
# bash scripts/monitor2psql.sh ompc_tpch_test vaultdb_monitoring
if([ $# -ne 2 ]); then
  echo "Usage: bash scripts/monitor2psql.sh <test name> <psql db name>"
  exit 1
fi

TOP_SRC_DIR='log/'$1
DB_NAME=$2

# attempt to create DB if it does not exist
#psql -U postgres -tc "SELECT 1 FROM pg_database WHERE datname = '$DB_NAME'" | grep -q 1 || psql -U postgres -c "CREATE DATABASE $DB_NAME"
dropdb --if-exists $DB_NAME

#check if failed to dropdb
if [ $? -ne 0 ]; then
  echo "Failed to drop database $DB_NAME"
  exit 1
fi

createdb $DB_NAME

# add tables
# timestamp (secs),active memory (bytes),swap (bytes),peak memory (bytes),rchar (bytes),"
  #                   "wchar (bytes),syscr (bytes),syscw (bytes),read_bytes,write_bytes,cancelled_write_bytes,"
  #                   "cpu time (clock ticks),network recvd (Bps),network sent (Bps),"
  #                   "memory bandwidth (MB/s),L3 cache misses,CPU stall cycles,IPC
psql $DB_NAME -c "CREATE TABLE IF NOT EXISTS performance(trial varchar, timestamp_secs INT, active_memory_bytes BIGINT, swap_bytes BIGINT, peak_memory_bytes BIGINT, rchar_bytes BIGINT, wchar_bytes BIGINT, syscr_bytes BIGINT, syscw_bytes BIGINT, read_bytes BIGINT, write_bytes BIGINT, cancelled_write_bytes BIGINT, cpu_time_clock_ticks BIGINT, network_recvd_bits_per_sec BIGINT, network_sent_bits_per_sec INT, memory_bandwidth_mbytes_per_sec BIGINT, l3_cache_misses INT, cpu_stall_cycles BIGINT, ipc FLOAT)" #instructions per cycle

psql $DB_NAME -c "CREATE TABLE IF NOT EXISTS query_offsets(trial varchar, test_id varchar, duration_secs float, time_offset_secs float)"

for TRIAL in $(ls $TOP_SRC_DIR/);
do
  ROOT=$TOP_SRC_DIR/$TRIAL
  PERF_FILE=$ROOT/performance.csv
  OFFSETS_FILE=$ROOT/query_offsets.csv
  # if both files exist, then we can import
  if [ -f "$PERF_FILE" ] && [ -f "$OFFSETS_FILE" ];
  then
    #delete extra header if needed
    awk 'FNR == 1; FNR > 1 && /^[0-9]/' $PERF_FILE > tmp && mv tmp $PERF_FILE
    psql $DB_NAME -c "\copy performance(timestamp_secs, active_memory_bytes, swap_bytes, peak_memory_bytes, rchar_bytes, wchar_bytes, syscr_bytes, syscw_bytes, read_bytes, write_bytes, cancelled_write_bytes, cpu_time_clock_ticks, network_recvd_bits_per_sec, network_sent_bits_per_sec, memory_bandwidth_mbytes_per_sec, l3_cache_misses, cpu_stall_cycles, ipc) FROM '$PERF_FILE' DELIMITER ',' CSV HEADER"
    CMD="UPDATE performance SET trial='$TRIAL' WHERE trial IS NULL"
    psql $DB_NAME -c "$CMD"
    psql $DB_NAME -c "\copy query_offsets(test_id, duration_secs, time_offset_secs) FROM '$OFFSETS_FILE' DELIMITER ',' CSV HEADER"
    CMD="UPDATE query_offsets SET trial='$TRIAL' WHERE trial IS NULL"
    psql $DB_NAME -c "$CMD"
  fi
done


psql vaultdb_monitoring -c "\COPY performance TO 'performance.csv' CSV HEADER"
psql vaultdb_monitoring -c "\COPY query_offsets TO 'query_offsets.csv' CSV HEADER"


