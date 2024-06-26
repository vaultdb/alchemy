#!/bin/bash

#usage: bash wire_pack_tpch_instance.sh <db name>
#e.g., bash wire_pack_tpch_instance.sh tpch_unioned_150

if [ $# -lt 1 ];
then
   printf "usage:  bash scripts/wire_pack_tpch_instance.sh  <db name>\n"
   printf "Not enough arguments - %d\n" $# 
   exit 0 
fi

DB_NAME=$1

make wire_pack_tpch_instance

[ -d wires/$DB_NAME ] || mkdir -p wires/$DB_NAME
./bin/wire_pack_tpch_instance $DB_NAME wires/$DB_NAME 1 > log/wire_pack_tpch_instance_p1.log &
./bin/wire_pack_tpch_instance $DB_NAME wires/$DB_NAME 2 > log/wire_pack_tpch_instance_p2.log &
./bin/wire_pack_tpch_instance $DB_NAME wires/$DB_NAME 3 > log/wire_pack_tpch_instance_p3.log &
./bin/wire_pack_tpch_instance $DB_NAME wires/$DB_NAME 10086 | tee log/wire_pach_tpch_instance_p10086.log
