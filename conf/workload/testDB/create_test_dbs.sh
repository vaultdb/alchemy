#!/bin/bash -x

#run from smcql home contains src/ test/
path=$(pwd)
echo "Path: $path"

dbPrefix='healthlnk'
dropdb $dbPrefix
createdb $dbPrefix
psql $dbPrefix -f $path/conf/schemas/healthlnk/plain.sql

for i in 1 2
do
    dbName=$dbPrefix'_site'$i
    dropdb $dbName
    createdb $dbName

    psql $dbName -f $path/conf/schemas/healthlnk/plain.sql
    psql $dbName -c "COPY diagnoses FROM '$path/conf/workload/testDB/$i/diagnoses.csv' WITH DELIMITER ','"
    psql $dbName -c "COPY medications FROM '$path/conf/workload/testDB/$i/medications.csv' WITH DELIMITER ','"
    psql $dbName -c "COPY site FROM '$path/conf/workload/testDB/$i/site.csv' WITH DELIMITER ','"

    val=$i
    if (($val == 1)); then
        psql $dbName -c "COPY remote_diagnoses FROM '$path/conf/workload/testDB/2/diagnoses.csv' WITH DELIMITER ','"
    else
        psql $dbName -c "COPY remote_diagnoses FROM '$path/conf/workload/testDB/1/diagnoses.csv' WITH DELIMITER ','"
    fi

    psql $dbName -f $path/conf/workload/testDB/registries.sql
done
