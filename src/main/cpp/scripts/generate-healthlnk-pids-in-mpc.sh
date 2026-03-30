#!/bin/bash

# Define database names
ALICE_DB="healthlnk_alice"
BOB_DB="healthlnk_bob"
HEALTHLNK_DB="healthlnk"

# Define file paths (use an absolute path for your temp folder)
TMP_DIR="$(pwd)/tmp"
mkdir -p "$TMP_DIR"

ALICE_DIAG="$TMP_DIR/alice_diag_pids.txt"
BOB_MED="$TMP_DIR/bob_med_pids.txt"
BOB_DIAG="$TMP_DIR/bob_diag_pids.txt"
ALICE_MED="$TMP_DIR/alice_med_pids.txt"
INTERSECT_1="$TMP_DIR/intersection_1.txt"
INTERSECT_2="$TMP_DIR/intersection_2.txt"
FINAL_PIDS="$TMP_DIR/in_mpc_pids.txt"

# Define padded file names
ALICE_DIAG_PADDED="$TMP_DIR/alice_diag_pids.txt.padded"
BOB_MED_PADDED="$TMP_DIR/bob_med_pids.txt.padded"
BOB_DIAG_PADDED="$TMP_DIR/bob_diag_pids.txt.padded"
ALICE_MED_PADDED="$TMP_DIR/alice_med_pids.txt.padded"

# Function to create the `pids_in_mpc` table if it does not exist
create_table_if_not_exists() {
    echo "🛠 Checking and creating 'pids_in_mpc' table if not exists..."
    for DB in $ALICE_DB $BOB_DB $HEALTHLNK_DB; do
        psql -d $DB -c "
        CREATE TABLE IF NOT EXISTS pids_in_mpc (
            patient_id INTEGER PRIMARY KEY
        );"
    done
}

# Function to dump patient IDs from PostgreSQL
dump_pids() {
    echo "🚀 Dumping PIDs from databases..."
    psql -d $ALICE_DB -c "\copy (SELECT DISTINCT patient_id FROM diagnoses ORDER BY patient_id) TO '$ALICE_DIAG' CSV;" \
        || { echo "❌ Failed to dump Alice Diagnoses"; exit 1; }
    psql -d $BOB_DB -c "\copy (SELECT DISTINCT patient_id FROM medications ORDER BY patient_id) TO '$BOB_MED' CSV;" \
        || { echo "❌ Failed to dump Bob Medications"; exit 1; }
    psql -d $BOB_DB -c "\copy (SELECT DISTINCT patient_id FROM diagnoses ORDER BY patient_id) TO '$BOB_DIAG' CSV;" \
        || { echo "❌ Failed to dump Bob Diagnoses"; exit 1; }
    psql -d $ALICE_DB -c "\copy (SELECT DISTINCT patient_id FROM medications ORDER BY patient_id) TO '$ALICE_MED' CSV;" \
        || { echo "❌ Failed to dump Alice Medications"; exit 1; }
}

# Function to pad the numbers so lexicographical order matches numeric order
pad_files() {
    echo "🔄 Padding files for numeric comparison..."
    awk '{printf "%06d\n", $1}' "$ALICE_DIAG" > "$ALICE_DIAG_PADDED"
    awk '{printf "%06d\n", $1}' "$BOB_MED" > "$BOB_MED_PADDED"
    awk '{printf "%06d\n", $1}' "$BOB_DIAG" > "$BOB_DIAG_PADDED"
    awk '{printf "%06d\n", $1}' "$ALICE_MED" > "$ALICE_MED_PADDED"

    # Sort the padded files (this is now a lexicographical sort, which works because of fixed width)
    sort "$ALICE_DIAG_PADDED" -o "$ALICE_DIAG_PADDED"
    sort "$BOB_MED_PADDED" -o "$BOB_MED_PADDED"
    sort "$BOB_DIAG_PADDED" -o "$BOB_DIAG_PADDED"
    sort "$ALICE_MED_PADDED" -o "$ALICE_MED_PADDED"
}

# Function to compute intersections and merge results
compute_intersections() {
    echo "🔄 Finding intersections..."

    # Use the padded files for comm so that comparison works numerically
    comm -12 "$ALICE_DIAG_PADDED" "$BOB_MED_PADDED" > "$INTERSECT_1"
    comm -12 "$BOB_DIAG_PADDED" "$ALICE_MED_PADDED" > "$INTERSECT_2"

    # Merge, remove duplicates, sort, then remove the padding (leading zeros)
    cat "$INTERSECT_1" "$INTERSECT_2" | sort -u | sed 's/^0*//' > "$FINAL_PIDS"

    echo "✅ Computed in_mpc PIDs stored in $FINAL_PIDS (Sorted and Deduplicated)"
}

# Function to load results into all PostgreSQL databases
load_into_db() {
    echo "📥 Loading in_mpc PIDs back into PostgreSQL databases..."

    if [[ -s "$FINAL_PIDS" ]]; then
        for DB in $ALICE_DB $BOB_DB $HEALTHLNK_DB; do
            psql -d $DB -c "TRUNCATE pids_in_mpc;"
            psql -d $DB -c "\copy pids_in_mpc(patient_id) FROM '$FINAL_PIDS' CSV;" \
                || { echo "❌ Failed to load PIDs into $DB"; exit 1; }
        done
        echo "✅ Successfully loaded in_mpc PIDs into healthlnk_alice, healthlnk_bob, and healthlnk"
    else
        echo "⚠️ Warning: No valid PIDs found. Skipping database insertion."
    fi
}

# Run all functions
create_table_if_not_exists
dump_pids
pad_files
compute_intersections
load_into_db

echo "🎉 Done! in_mpc PIDs are now in all PostgreSQL databases."