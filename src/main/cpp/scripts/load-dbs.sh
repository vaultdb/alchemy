#!/bin/bash

#depends on vaultdb-test-dbs.tgz being in this directory

mkdir dbs
rm -rf dbs/*
tar -zxvf  vaultdb-test-dbs.tgz 
for f in $(ls dbs)
do
     DB_NAME=$(echo $f | sed 's/.sql//g')
     dropdb $DB_NAME
     createdb $DB_NAME
     psql $DB_NAME < dbs/$f
done
