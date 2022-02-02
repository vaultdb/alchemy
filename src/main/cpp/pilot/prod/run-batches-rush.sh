#!/bin/bash -x



for YEAR in 2018 2019 2020 all
do
    echo 'Evaluating study_year='$YEAR
    ./pilot/prod/run-data-partner-aggregation-rush.sh $YEAR
    ./pilot/prod/run-data-partner-rush.sh $YEAR
    for B in 10 20 30 40 50
    do
	
	echo 'Running for '$YEAR' batch count: '$B
	./pilot/prod/run-data-partner-batch-rush.sh $YEAR $B
    done
done

