#!/bin/bash

executable="./bin/secure_aggregate_comparison_test"
PARTY=2

# Get today's date
date=$(date +'%Y-%m-%d')

# Define the cutoff values
cutoff_values=(151 301 601)

dbnames=("tpch_unioned_150" "tpch_unioned_300" "tpch_unioned_600")

log_file="AggregateComparison_${date}.log"

for ((i=0; i<${#cutoff_values[@]}; i++))
do
    echo "Running tests with Bit Packed" >> "$log_file"
    $executable --party=$PARTY --alice_host=129.105.61.182 --cutoff=${cutoff_values[$i]} --dbname=${dbnames[$i]} --bitpacking="packed" >> "$log_file"

    echo "Running tests with Non Bit Packed" >> "$log_file"
    $executable --party=$PARTY --alice_host=129.105.61.182 --cutoff=${cutoff_values[$i]} --dbname=${dbnames[$i]} --bitpacking="non" >> "$log_file"
done