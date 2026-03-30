#!/bin/bash

exec1="./bin/secure_cloud_setting_test"
PARTY=1

# Today's date
date=$(date +'%Y-%m-%d')

unioned_dbs=("tpch_unioned_150" "tpch_unioned_1500")
alice_dbs=("tpch_alice_150" "tpch_alice_1500")
bob_dbs=("tpch_bob_150" "tpch_bob_1500")

# First batch
LOG_DIR1="./log/cloud_setting_test"
mkdir -p "$LOG_DIR1"

echo "Running 22 cost-optimized tests for Alice (Party $PARTY)"

for ((i=0; i<${#unioned_dbs[@]}; i++)); do
    for test_name in cost_optimized_tpch_q{1..22}; do
        LOG_FILE="$LOG_DIR1/alice_${test_name}_${unioned_dbs[$i]}.log"
        echo "Running Alice test: ${test_name} on ${unioned_dbs[$i]}..."

        env PGHOST=127.0.0.1 PGUSER=vaultdb PGPASSWORD="xlr6007" \
            $exec1 --party=$PARTY \
            --unioned_db=${unioned_dbs[$i]} \
            --alice_db=${alice_dbs[$i]} \
            --bob_db=${bob_dbs[$i]} \
            --filter="*${test_name}" > "$LOG_FILE" 2>&1
    done
done
