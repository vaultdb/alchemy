#!/bin/bash

PARTY=2
ALICE_HOST="126.137.1.10"

# First batch: cost_optimized_tpch_q1..22
exec1="./bin/generalized_comparison_test"
LOG_DIR1="./log/generalized_comparison"
mkdir -p "$LOG_DIR1"

echo "Running 22 cost-optimized tests for Bob (Party $PARTY)"

for test_name in cost_optimized_tpch_q{1..22}; do
    LOG_FILE="$LOG_DIR1/bob_${test_name}.log"
    echo "Running Bob test: ${test_name}..."

    env PGHOST=127.0.0.1 PGUSER=vaultdb PGPASSWORD="xlr6007" \
        $exec1 --party=$PARTY --alice_host=$ALICE_HOST \
        --unioned_db=tpch_unioned_1500 \
        --alice_db=tpch_alice_1500 --bob_db=tpch_bob_1500 \
        --filter="*${test_name}" > "$LOG_FILE" 2>&1
done

# Second batch: left_deep_tpch_q5,8,9
exec2="./bin/cost_optimized_test"
LOG_DIR2="./log/VLDB_additional"
mkdir -p "$LOG_DIR2"

echo "Running 3 left-deep tests for Bob (Party $PARTY)"

for q in 5 8 9; do
    test_name="left_deep_tpch_q$q"
    LOG_FILE="$LOG_DIR2/bob_${test_name}.log"
    echo "Running Bob test: ${test_name}..."

    env PGHOST=127.0.0.1 PGUSER=vaultdb PGPASSWORD="xlr6007" \
        $exec2 --party=$PARTY --alice_host=$ALICE_HOST \
        --unioned_db=tpch_unioned_1500 \
        --alice_db=tpch_alice_1500 --bob_db=tpch_bob_1500 \
        --filter="*${test_name}" > "$LOG_FILE" 2>&1
done
