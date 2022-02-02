#!/bin/bash -x



for YEAR in 2018 2019 2020 all
do

    for B in 10 20 30 40 50
    do
	
	echo 'Running for '$YEAR' batch count: '$B
	./pilot/prod/run-data-partner-batch-nm.sh $YEAR $B
    done
done

