#!/bin/bash

executable="./bin/baseline_comparison_test"
PARTY=1

echo "Running tests with test=baseline"
$executable --party=$PARTY --test=baseline > baseline_comparison_test${PARTY}_baseline.log

echo "Running tests with test=handcode"
$executable --party=$PARTY --test=handcode > baseline_comparison_test${PARTY}_handcode.log
