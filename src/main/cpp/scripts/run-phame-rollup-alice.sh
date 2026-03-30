#!/bin/bash

#aggregate-only
mkdir -p pilot/results/phame/batch/phame_rollup
time ./bin/catalyst 1 pilot/prod/study/phame/batch/phame_rollup/study.json  2>&1  | tee log/phame/batch/phame_rollup_alice.log

for SITE_ID in 0 1 2 5
do
    
    mkdir -p pilot/results/phame/batch/row_level/$SITE_ID
    time ./bin/catalyst 1  pilot/prod/study/phame/batch/row_level/$SITE_ID/study.json 2>&1  | tee 'log/phame/batch/row_level_'$SITE_ID'_alice.log'
done


PARTY=alice
mkdir -p pilot/results/phame/batch/unioned/0
cat pilot/results/phame/batch/phame_rollup/PHAME_ROLLUP.$PARTY  \
	pilot/results/phame/batch/row_level/0/phame_diagnosis_rollup.$PARTY \ 
	pilot/results/phame/batch/row_level/1/phame_diagnosis_rollup.$PARTY \
	pilot/results/phame/batch/row_level/2/phame_diagnosis_rollup.$PARTY \ 
	pilot/results/phame/batch/row_level/5/phame_diagnosis_rollup.$PARTY > \
	pilot/results/phame/batch/unioned/0/phame_rollup.$PARTY

       
