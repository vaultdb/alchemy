#!/bin/bash

# Reset cache
sudo sync
echo 3 | sudo tee /proc/sys/vm/drop_caches

# Specify your disk identifier (e.g., sda)
DISK_ID="sda"

# Function to get read and write I/O in sectors from /proc/diskstats
get_disk_stats() {
    awk -v disk="$DISK_ID" '$3 == disk {print $4, $5, $8, $9}' /proc/diskstats
}

# Function to ger read and write amounts in kB ,and read and write speed in kB/s by iostat
get_io_stats() {
    iostat | grep $DISK_ID | awk '{print $3, $4, $6, $7}'
}

KB_TO_MB_FACTOR=1000

# Capture initial I/O stats
cat /proc/diskstats | grep " sda "
read INITIAL_READS_COMPLETED INITIAL_READS_MERGED INITIAL_WRITES_COMPLETED INITIAL_WRITES_MERGED < <(get_disk_stats)
echo "Initial reads completed: $INITIAL_READS_COMPLETED, initial reads merged: $INITIAL_READS_MERGED, initial writes completed: $INITIAL_WRITES_COMPLETED, initial writes merged: $INITIAL_WRITES_MERGED"

iostat | grep sda
read INITIAL_READ_SIZE INITIAL_WRITE_SIZE INITIAL_READ_SPEED INITIAL_WRITE_SPEED < <(get_io_stats)
INITIAL_READ_SIZE_MB=$(echo "$INITIAL_READ_SIZE / $KB_TO_MB_FACTOR" | bc -l)
INITIAL_WRITE_SIZE_MB=$(echo "$INITIAL_WRITE_SIZE / $KB_TO_MB_FACTOR" | bc -l)
INITIAL_READ_SPEED_MBPS=$(echo "$INITIAL_READ_SPEED / $KB_TO_MB_FACTOR" | bc -l)
INITIAL_WRITE_SPEED_MBPS=$(echo "$INITIAL_WRITE_SPEED / $KB_TO_MB_FACTOR" | bc -l)
echo "Initial read size: $INITIAL_READ_SIZE_MB MB, initial write size: $INITIAL_WRITE_SIZE_MB MB, initial read speed: $INITIAL_READ_SPEED_MBPS MB/s, initial write speed: $INITIAL_WRITE_SPEED_MBPS MB/s"

# Run the program with memory restrictions
#sudo cgexec -g memory:my_cgroup ./bin/ompc_block_nested_loop_join_test --party=1 --validation=false --input_size=s --storage=wire_packed --unioned_db=tpch_unioned_300 --filter="*.test_tpch_q3_customer_orders"
$1 --party=$2 --validation=false --input_size=$3 --storage=$4 --unioned_db=$5 --filter=$6 --unpacked_page_size_bits=$7 --page_cnt=$8

iostat | grep sda
read FINAL_READ_SIZE FINAL_WRITE_SIZE FINAL_READ_SPEED FINAL_WRITE_SPEED < <(get_io_stats)
FINAL_READ_SIZE_MB=$(echo "$FINAL_READ_SIZE / $KB_TO_MB_FACTOR" | bc -l)
FINAL_WRITE_SIZE_MB=$(echo "$FINAL_WRITE_SIZE / $KB_TO_MB_FACTOR" | bc -l)
FINAL_READ_SPEED_MBPS=$(echo "$FINAL_READ_SPEED / $KB_TO_MB_FACTOR" | bc -l)
FINAL_WRITE_SPEED_MBPS=$(echo "$FINAL_WRITE_SPEED / $KB_TO_MB_FACTOR" | bc -l)
echo "Final read size: $FINAL_READ_SIZE_MB MB, final write size: $FINAL_WRITE_SIZE_MB MB, final read speed: $FINAL_READ_SPEED_MBPS MB/s, final write speed: $FINAL_WRITE_SPEED_MBPS MB/s"

# Capture final I/O stats
cat /proc/diskstats | grep " sda "
read FINAL_READS_COMPLETED FINAL_READS_MERGED FINAL_WRITES_COMPLETED FINAL_WRITES_MERGED < <(get_disk_stats)
echo "Final reads completed: $FINAL_READS_COMPLETED, Final reads merged: $FINAL_READS_MERGED, Final writes completed: $FINAL_WRITES_COMPLETED, Final writes merged: $FINAL_WRITES_MERGED"

TOTAL_READS=$((FINAL_READS_COMPLETED+FINAL_READS_MERGED-INITIAL_READS_COMPLETED-INITIAL_READS_MERGED))
TOTAL_WRITES=$((FINAL_WRITES_COMPLETED+FINAL_WRITES_MERGED-INITIAL_WRITES_COMPLETED-INITIAL_WRITES_MERGED))

TOTAL_READ_SIZE_MB=$(echo "$FINAL_READ_SIZE_MB - $INITIAL_READ_SIZE_MB" | bc -l)
TOTAL_WRITE_SIZE_MB=$(echo "$FINAL_WRITE_SIZE_MB - $INITIAL_WRITE_SIZE_MB" | bc -l)
#Final speeds capture average I/O speeds during execution
TOTAL_READ_TIME=$(echo "$TOTAL_READ_SIZE_MB / $FINAL_READ_SPEED_MBPS" | bc -l)
TOTAL_WRITE_TIME=$(echo "$TOTAL_WRITE_SIZE_MB / $FINAL_WRITE_SPEED_MBPS" | bc -l)

echo "Total read size: $TOTAL_READ_SIZE_MB MB, total read time: $TOTAL_READ_TIME s, average read speed: $FINAL_READ_SPEED_MBPS MB/s, total write size: $TOTAL_WRITE_SIZE_MB MB, total write time: $TOTAL_WRITE_TIME s, average write speed: $FINAL_WRITE_SPEED_MBPS MB/s"

# Output total reads and writes
echo "Total Reads: $TOTAL_READS, Total Writes: $TOTAL_WRITES"
