#!/bin/bash

#usage: bash secret_share_tpch_instance.sh <db name>
#e.g., bash secret_share_tpch_instance.sh tpch_unioned_150

if [ $# -lt 1 ];
then
   printf "usage:  bash scripts/secret_share_tpch_instance.sh  <db name>\n"
   printf "Not enough arguments - %d\n" $#
   exit 0
fi

DB_NAME=$1
PARTY_COUNT=4

make -j4 secret_share_tpch_instance


if [[ $? -ne 0 ]]; then #abort if build fails
    exit 1
fi


[ -d shares/$DB_NAME ] || mkdir -p shares/$DB_NAME
./bin/secret_share_tpch_instance $DB_NAME shares/$DB_NAME 1 > log/secret_share_tpch_instance_p1.log &
./bin/secret_share_tpch_instance $DB_NAME shares/$DB_NAME 2 > log/secret_share_tpch_instance_p2.log &
./bin/secret_share_tpch_instance $DB_NAME shares/$DB_NAME 3 > log/secret_share_tpch_instance_p3.log &
./bin/secret_share_tpch_instance $DB_NAME shares/$DB_NAME 10086 | tee log/secret_share_tpch_instanc_p10086.log
