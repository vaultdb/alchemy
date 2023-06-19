#!/bin/bash

rm log/*.csv
for FILE in $(ls log/all-tests-alice-*.log)
   do
     OUT_FILE=$(echo $FILE | sed 's/.log/.csv/')
     python parse-log.py -input $FILE -output $OUT_FILE
done

for FILE in $(ls log/all-tests-bob-*.log)
   do
     OUT_FILE=$(echo $FILE | sed 's/.log/.csv/')
     python parse-log.py -input $FILE -output $OUT_FILE
done


cat log/*.csv > temp.csv
mv temp.csv log/all-entries.csv
createdb vaultdb_benchmarking 
psql vaultdb_benchmarking < unit-test-benchmarking.sql 
