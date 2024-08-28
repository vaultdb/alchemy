#!/bin/bash

# Reset cache
sudo sync
echo 3 | sudo tee /proc/sys/vm/drop_caches

# Specify your disk identifier (e.g., sda)
DISK_ID="sda"

# Function to get read and write I/O in sectors from /proc/diskstats
get_io_stats() {
    awk -v disk="$DISK_ID" '$3 == disk {print $4, $5, $8, $9}' /proc/diskstats
}

# Capture initial I/O stats
read INITIAL_READS_COMPLETED INITIAL_READS_MERGED INITIAL_WRITES_COMPLETED INITIAL_WRITES_MERGED < <(get_io_stats)
echo "Initial reads completed: $INITIAL_READS_COMPLETED, initial reads merged: $INITIAL_READS_MERGED, initial writes completed: $INITIAL_WRITES_COMPLETED, initial writes merged: $INITIAL_WRITES_MERGED"

# Run the program with memory restrictions
#sudo cgexec -g memory:my_cgroup ./bin/ompc_block_nested_loop_join_test --party=1 --validation=false --input_size=s --storage=wire_packed --unioned_db=tpch_unioned_300 --filter="*.test_tpch_q3_customer_orders"
$1 --party=1 --validation=false --input_size=$2 --storage=$3 --unioned_db=$4 --filter=$5

# Capture final I/O stats
read FINAL_READS_COMPLETED FINAL_READS_MERGED FINAL_WRITES_COMPLETED FINAL_WRITES_MERGED < <(get_io_stats)
echo "Final reads completed: $FINAL_READS_COMPLETED, Final reads merged: $FINAL_READS_MERGED, Final writes completed: $FINAL_WRITES_COMPLETED, Final writes merged: $FINAL_WRITES_MERGED"

TOTAL_READS=$((FINAL_READS_COMPLETED+FINAL_READS_MERGED-INITIAL_READS_COMPLETED-INITIAL_READS_MERGED))
TOTAL_WRITES=$((FINAL_WRITES_COMPLETED+FINAL_WRITES_MERGED-INITIAL_WRITES_COMPLETED-INITIAL_WRITES_MERGED))

# Output total reads and writes
echo "Total Reads: $TOTAL_READS, Total Writes: $TOTAL_WRITES"
