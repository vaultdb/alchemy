#!/bin/bash

PARTY=$1
UNIONED_DB=$2
ALICE_DB=$3
BOB_DB=$4
FILTER_NAME=$5
ALICE_HOST=$6

exec1="./bin/secure_cloud_setting_test"

# Always use full path for log directory to avoid relative path issues
LOG_DIR="/home/cloud_mpc/vaultdb-core/src/main/cpp/log/cloud_setting_test"
mkdir -p "$LOG_DIR"

ROLE="alice"
if [[ "$PARTY" == "2" ]]; then
  ROLE="bob"
fi

LOG_FILE="$LOG_DIR/${ROLE}_${FILTER_NAME}_${UNIONED_DB}.log"

echo "Running ${ROLE^} test on DB: $UNIONED_DB, Filter: $FILTER_NAME"

CMD="env PGHOST=127.0.0.1 PGUSER=vaultdb PGPASSWORD='xlr6007' \
    $exec1 --party=$PARTY"

if [[ "$PARTY" == "2" ]]; then
  CMD="$CMD --alice_host=$ALICE_HOST"
fi

CMD="$CMD \
    --unioned_db=$UNIONED_DB \
    --alice_db=$ALICE_DB \
    --bob_db=$BOB_DB \
    --filter=*$FILTER_NAME"

# Run the command and write output to log using sudo tee
eval "$CMD" 2>&1 | sudo tee "$LOG_FILE" > /dev/null
