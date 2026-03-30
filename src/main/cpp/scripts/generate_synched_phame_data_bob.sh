#!/bin/bash

bash pilot/test/generate-and-load-phame-test-data.sh 4 100

 for SRC_FILE in $(ls pilot/secret_shares/output/*/* pilot/study/phame/expected/*.csv)
do
    echo $SRC_FILE
   nc -q 0 127.0.0.1 4444 < $SRC_FILE
    sleep 0.5
done