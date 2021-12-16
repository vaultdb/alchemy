#!/bin/bash

#run this before loading the data
for f in $(ls pilot/input/*);
do
    sed -i 's/,NULL,/,,/g' $f;
    sed -i 's/,NULL$/,/g' $f;
done
