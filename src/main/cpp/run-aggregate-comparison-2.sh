#!/bin/bash

executable="./bin/secure_aggregate_comparison_test"
PARTY=2

# Get today's date
date=$(date +'%Y-%m-%d')

# Define the cutoff values
cutoff_values=(10 100 1000)

log_file="AggregateComparison_${date}.log"

for cutoff in "${cutoff_values[@]}"
do
    echo "Running tests with Bit Packed" >> "$log_file"
    $executable --party=$PARTY --cutoff=$cutoff --bitpacking="packed" >> "$log_file"

    echo "Running tests with Non Bit Packed" >> "$log_file"
        $executable --party=$PARTY --cutoff=$cutoff --bitpacking="non" >> "$log_file"
done
