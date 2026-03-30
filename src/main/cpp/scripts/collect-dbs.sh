#!/bin/bash

mkdir dbs
rm dbs/*
for db in $(psql -tU vaultdb postgres -c "SELECT datname  FROM pg_database ORDER BY datname" | egrep -v 'template0|template1|postgres|jennie'); do 
   echo $db ; pg_dump -U vaultdb $db > dbs/$db.sql;
 done
tar -czf vaultdb-test-dbs.tgz dbs/*.sql
