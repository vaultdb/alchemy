# e.g., bash perf_script.sh ompc_basic_join_test false "*.test_tpch_q3_customer_orders" "wire_packed" flamegraph.svg
/usr/bin/perf record -F 100 -a --call-graph  dwarf -m256M ../bin/$1 --party=1 --validation=$2 --filter=$3 --storage=$4
/usr/bin/perf script > out.perf  
/home/vaultdb/FlameGraph/stackcollapse-perf.pl out.perf > out.folded 
/home/vaultdb/FlameGraph/flamegraph.pl out.folded > $5