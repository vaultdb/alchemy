#!/bin/bash

#Check usage
if [ "$#" -ne 1 ]; then
  printf "Usage: ./build_and_execute.sh <arg1>\n\targ1: Path (including file name) containing SQL query. E.g. conf/workload/sql/comorbidity.sql\n"
  exit 1
fi

#Build jar
echo "Creating executable..."
mvn package -Dmaven.test.skip=true >/dev/null

if (($?==1)); then
    echo "Error creating executable"
    exit 1
fi

#Execute query
query=$(<$1)
echo "Executing Query..."
java -cp "target/smcql-open-source-0.5.jar" org.smcql.runner.SMCQLRunner "$query"
