#!/bin/bash

#setup test databases for unit tests
echo "Populating test databases..."
/bin/bash conf/workload/testDB/create_test_dbs.sh

if (($?==1)); then
    echo "Error populating test databases"
    exit 1
fi

# needed for macosx, may need to check for os name on this first:
ln -s /usr/local/opt/openssl/include/openssl /usr/local/include
ln -s /usr/local/opt/openssl/lib/libssl.1.0.0.dylib /usr/local/lib/

#download emp to dep
/bin/bash deps/emp/install.sh 

#end
echo "Setup completed successfully."
exit 0
