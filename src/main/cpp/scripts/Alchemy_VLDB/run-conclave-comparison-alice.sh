#!/bin/bash

set -e
executable="./bin/conclave_comparison_test"
LOG_DIR="./log/conclave_comparison"
GEN_SCRIPT="./scripts/generate-and-load-healthlnk.sh"
JSON_PATH="./conf/datagen/healthlnk.json"

mkdir -p "$LOG_DIR"

CARD_LIST=(100 1000 10000 100000 1000000)

for CARD in "${CARD_LIST[@]}"; do
  echo "🛠️  Updating cardinality to $CARD in $JSON_PATH"

   # Replace only the first two "cardinality" fields (diagnoses and medications)
    awk -v card="$CARD" '
      BEGIN { count = 0 }
      {
        if ($0 ~ /"cardinality"/ && count < 2) {
          sub(/[0-9]+/, card)
          count++
        }
        print
      }
    ' "$JSON_PATH" > "${JSON_PATH}.tmp" && mv "${JSON_PATH}.tmp" "$JSON_PATH"

  echo "📦 Generating data for cardinality $CARD..."
  bash "$GEN_SCRIPT"

  for FILTER in "*.conclave" "*.alchemy"; do
    TEST_NAME=$(echo "$FILTER" | cut -d'.' -f2)
    LOG_FILE="$LOG_DIR/alice_card_${CARD}_${TEST_NAME}.log"
    echo "🚀 Running $TEST_NAME test for cardinality $CARD..."
    env PGHOST=127.0.0.1 PGUSER=vaultdb PGPASSWORD="xlr6007" \
      $executable --party=1 \
      --unioned_db=healthlnk_unioned_${CARD} \
      --alice_db=healthlnk_alice_${CARD} \
      --bob_db=healthlnk_bob_${CARD} \
      --filter="$FILTER" > "$LOG_FILE" 2>&1
    echo "✅ Finished $TEST_NAME test for card=$CARD. Log: $LOG_FILE"
  done

done

echo -e "\n🎉 All tests complete!"
