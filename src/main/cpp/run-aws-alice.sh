#!/bin/bash

# Get today's date in YYYY-MM-DD format
today=$(date +"%Y-%m-%d")

# Define the necessary variables
party=1
unioned_db="tpch_unioned_1500"
alice_db="tpch_alice_1500"
bob_db="tpch_bob_1500"
log_file="${today}_Cost_optimized_Alice_${unioned_db}.log"

# Function to run tests and log output
run_test() {
  test_name=$1
  echo "Running $test_name..." | tee -a "$log_file"
  sudo ./bin/baseline_comparison_test --party=$party --unioned_db=$unioned_db --alice_db=$alice_db --bob_db=$bob_db --gtest_filter="$test_name" 2>&1 | tee -a "$log_file"
  echo "Finished $test_name." | tee -a "$log_file"
  echo "---------------------------------" | tee -a "$log_file"
}

# Start running all the tests one by one and log to the file
echo "Starting tests on $(date)" | tee "$log_file"
echo "Logging output to $log_file"

run_test "CostOptimizedTest.card_bound_tpch_q1"
run_test "CostOptimizedTest.card_bound_tpch_q3"
run_test "CostOptimizedTest.card_bound_tpch_q5"
run_test "CostOptimizedTest.card_bound_tpch_q8"
run_test "CostOptimizedTest.card_bound_tpch_q9"
run_test "CostOptimizedTest.card_bound_tpch_q18"

run_test "CostOptimizedTest.bushy_plan_tpch_q5"
run_test "CostOptimizedTest.bushy_plan_tpch_q8"
run_test "CostOptimizedTest.bushy_plan_tpch_q9"

run_test "CostOptimizedTest.cost_optimized_tpch_q1"
run_test "CostOptimizedTest.cost_optimized_tpch_q3"
run_test "CostOptimizedTest.cost_optimized_tpch_q5"
run_test "CostOptimizedTest.cost_optimized_tpch_q8"
run_test "CostOptimizedTest.cost_optimized_tpch_q9"
run_test "CostOptimizedTest.cost_optimized_tpch_q18"

echo "All tests completed on $(date)" | tee -a "$log_file"

