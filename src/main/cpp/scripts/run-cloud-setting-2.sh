#!/bin/bash

exec1="./bin/secure_cloud_setting_test"
PARTY=2
ALICE_HOST="126.137.1.10"

# Today's date
date=$(date +'%Y-%m-%d')

unioned_dbs=("tpch_unioned_150" "tpch_unioned_1500")
alice_dbs=("tpch_alice_150" "tpch_alice_1500")
bob_dbs=("tpch_bob_150" "tpch_bob_1500")

filters=("secure_cloud_setting_tpch_q1" "secure_cloud_setting_tpch_q3" "secure_cloud_setting_tpch_q5" "secure_cloud_setting_tpch_q8" "secure_cloud_setting_tpch_q9" "secure_cloud_setting_tpch_q18")

LOG_DIR="./log/cloud_setting_test"
mkdir -p "$LOG_DIR"

echo "Running cloud setting tests for Bob (Party $PARTY)"

for ((i=0; i<${#unioned_dbs[@]}; i++)); do
    for test_name in "${filters[@]}"; do
        LOG_FILE="$LOG_DIR/bob_${test_name}_${unioned_dbs[$i]}.log"
        echo "Running Bob test on DB: ${unioned_dbs[$i]}, Filter: ${test_name}"

        # Clear cache before running the test
        echo "Clearing system cache..."
        sudo sync
        echo 3 | sudo tee /proc/sys/vm/drop_caches

        env PGHOST=127.0.0.1 PGUSER=vaultdb PGPASSWORD="xlr6007" \
            $exec1 --party=$PARTY --alice_host=$ALICE_HOST \
            --unioned_db=${unioned_dbs[$i]} \
            --alice_db=${alice_dbs[$i]} \
            --bob_db=${bob_dbs[$i]} \
            --filter="*${test_name}" > "$LOG_FILE" 2>&1
    done
done