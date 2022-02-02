#!/bin/bash -x


LOG=pilot/logs/perf-measurements.log
chmod u+x pilot/prod/*.sh

#    ./pilot/prod/run-data-partner-aggregation-nm.sh $YEAR 
#    sleep 2
#    ./pilot/prod/run-data-partner-nm.sh $YEAR  >> $LOG

echo "Starting new run" >> $LOG
for YEAR in 2018 2019 2020 all
do
    echo 'Evaluating study_year='$YEAR
    for B in 50 40 30 20 10
    do
	echo 'Running for '$YEAR' batch count: '$B
	bash pilot/prod/run-data-partner-batch-nm.sh $YEAR $B  >> $LOG
	sleep 2
    done
done

