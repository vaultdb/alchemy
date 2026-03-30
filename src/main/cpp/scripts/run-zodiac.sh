#!/bin/bash

#cd vaultdb-core/src/main/cpp
cmake .
bash  pilot/test/generate-and-load-zodiac-test-data.sh

make -j4 catalyst

# Run the zodiac query
./bin/catalyst 1 pilot/study/zodiac/study.json > log/zodiac_alice.log &
./bin/catalyst 2 pilot/study/zodiac/study.json | tee log/zodiac_bob.log

bash pilot/study/zodiac/reveal_and_postprocess_results.sh
