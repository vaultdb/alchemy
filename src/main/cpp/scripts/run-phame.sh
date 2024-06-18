#!/bin/bash

bash  pilot/test/generate-and-load-phame-test-data.sh 4 100

make -j4 catalyst

# Run the phame query
./bin/catalyst 1 pilot/study/phame/study.json &
./bin/catalyst 2 pilot/study/phame/study.json
