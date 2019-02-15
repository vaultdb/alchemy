#!/bin/bash

#setup test databases for unit tests
echo "Populating test databases..."
/bin/bash conf/workload/testDB/create_test_dbs.sh

if (($?==1)); then
    echo "Error populating test databases"
    exit 1
fi

#download emp to dep
/bin/bash deps/emp/install.sh 

#end
echo "Setup completed successfully."
exit 0
