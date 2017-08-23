#!/bin/bash

java -cp "target/lib/*:target/smcql-calcite-0.5.jar" org.smcql.runner.SMCQLRunner "$1"
