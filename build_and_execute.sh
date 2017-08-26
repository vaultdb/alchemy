#!/bin/bash

#Check usage
if [ "$#" -ne 2 ]; then
  printf "Usage: ./build_and_execute.sh <arg1> <arg2>\n\targ1: Name of executable. E.g. smcql-open-source-0.5.jar\n\targ2: Path (including file name) containing SQL query. E.g. conf/workload/sql/comorbidity.sql\n"
  exit 1
fi

#Build jar
echo "Creating executable..."
mvn package -Dmaven.test.skip=true

if (($?==1)); then
    echo "Error creating executable"
    exit 1
fi

#Execute query
query=$(<$2)
echo "Executing Query..."
java -cp "target/lib/*:target/$1" org.smcql.runner.SMCQLRunner "$query"
