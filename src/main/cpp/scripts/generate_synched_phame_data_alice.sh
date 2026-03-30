#!/bin/bash
# set up the dst filenames
bash pilot/test/generate-and-load-phame-test-data.sh 4 100

 for DST_FILE in $(ls pilot/secret_shares/output/*/* pilot/study/phame/expected/*.csv)
do
    echo $DST_FILE
    nc -l -p 4444 > $DST_FILE
done