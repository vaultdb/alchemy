#!/bin/bash

rm log/*.csv
for FILE in $(ls log/all-tests-alice-*.log log/all-tests-bob-*.log)
   do
     OUT_FILE=$(echo $FILE | sed 's/.log/.csv/')
     python scripts/parse-log.py -input $FILE -output $OUT_FILE
done


cat log/*.csv > temp.csv
mv temp.csv log/all-entries.csv
dropdb vaultdb_benchmarking
createdb vaultdb_benchmarking 
psql vaultdb_benchmarking < unit-test-benchmarking.sql 
