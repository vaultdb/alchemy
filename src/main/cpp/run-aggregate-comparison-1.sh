#!/bin/bash

executable="./bin/secure_aggregate_comparison_test"
PARTY=1

# Get today's date
date=$(date +'%Y-%m-%d')


unioned_dbs=("tpch_unioned_150" "tpch_unioned_300" "tpch_unioned_600")
alice_dbs=("tpch_alice_150" "tpch_alice_300" "tpch_alice_600")
bob_dbs=("tpch_bob_150" "tpch_bob_300" "tpch_bob_600")

log_file="AggregateComparison_${date}.log"

for ((i=0; i<${#unioned_dbs[@]}; i++))
do
    echo "Running tests with Bit Packed" >> "$log_file"
    $executable --party=$PARTY --cutoff=${cutoff_values[$i]} --unioned_db=${unioned_dbs[$i]} --alice_db=${alice_dbs[$i]} --bob_db=${bob_dbs[$i]} --bitpacking="packed" >> "$log_file"

    echo "Running tests with Non Bit Packed" >> "$log_file"
    $executable --party=$PARTY --cutoff=${cutoff_values[$i]} --unioned_db=${unioned_dbs[$i]} --alice_db=${alice_dbs[$i]} --bob_db=${bob_dbs[$i]} --bitpacking="non" >> "$log_file"
done
