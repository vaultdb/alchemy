#!/bin/bash -x


LOG=pilot/logs/perf-measurements.log
chmod u+x pilot/prod/*.sh

echo "Starting new run" >> $LOG
for YEAR in 2018 2019 2020 all
do
    echo 'Evaluating study_year='$YEAR
    ./pilot/prod/run-data-partner-aggregation-nm.sh $YEAR 
    sleep 2
    ./pilot/prod/run-data-partner-nm.sh $YEAR  >> $LOG
    for B in 10 20 30 40 50
    do
	sleep 2
	echo 'Running for '$YEAR' batch count: '$B
	./pilot/prod/run-data-partner-batch-nm.sh $YEAR $B  >> $LOG
    done
done

