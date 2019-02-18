#!/bin/bash

EMP_BRIDGE_PATH="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"
EMP_CLASS=$1
EMP_SH2PC=$EMP_BRIDGE_PATH'../emp-sh2pc'
SRC_FILE=$EMP_BRIDGE_PATH'/src/'$EMP_CLASS'.cpp'
SHARED_OBJECT=$EMP_BRIDGE_PATH/bin/$EMP_CLASS'.so'


echo "Emp bridge dir " $EMP_BRIDGE_PATH
#caution - need to generalize sdk for linux


#build
/usr/bin/clang++  -DEMP_CIRCUIT_PATH=/usr/local/include/emp-tool/circuits/files/ -I$EMP_SH2PC  -I/usr/local/include  -pthread -Wall -march=native -O3 -maes -mrdseed -std=c++11 -O3 -DNDEBUG -isysroot /Library/Developer/CommandLineTools/SDKs/MacOSX10.14.sdk   -o $SHARED_OBJECT -c $SRC_FILE
#link
/usr/bin/clang++  -pthread -Wall -march=native -O3 -maes -mrdseed -std=c++11 -O3 -DNDEBUG -isysroot /Library/Developer/CommandLineTools/SDKs/MacOSX10.14.sdk -Wl,-search_paths_first -Wl,-headerpad_max_install_names  $SHARED_OBJECT  -o bin/$EMP_CLASS -Wl,-rpath,/usr/local/lib /usr/local/lib/librelic.dylib /usr/lib/libssl.dylib /usr/lib/libcrypto.dylib /usr/local/lib/libboost_system-mt.dylib /usr/local/lib/libgmp.dylib /usr/local/lib/libemp-tool.dylib 

