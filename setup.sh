#!/bin/bash

#setup test databases for unit tests
#echo "Populating test databases..."
#/bin/bash conf/workload/testDB/create_test_dbs.sh
bin/bash conf/workload/tpch/generate-and-load-data.sh

#if (($?==1)); then
#    echo "Error populating test databases"
#    exit 1
#fi

# needed for macosx, may need to check for os name on this first:
ln -s /usr/local/opt/openssl/include/openssl /usr/local/include
ln -s /usr/local/opt/openssl/lib/libssl.1.0.0.dylib /usr/local/lib/
ln -s /usr/local/opt/openssl/lib/libcrypto.dylib /usr/local/lib/
ln -s /usr/local/opt/openssl/lib/libssl.dylib /usr/local/lib/

#download emp to dep
cd deps/emp
/bin/bash install.sh 

#install correct pqxx version

cd ..
git clone --branch 7.7.4 https://github.com/jtv/libpqxx.git
cd libpqxx
 ./configure  --disable-documentation --enable-shared CXXFLAGS="-std=c++17 -O2"
make 
sudo make install


#end
echo "Setup completed successfully."
exit 0
