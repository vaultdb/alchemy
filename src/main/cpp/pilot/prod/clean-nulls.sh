#!/bin/bash

#run this before loading the data
for f in $(ls pilot/input/nm/*.csv);
do
    dos2unix $f
    sed -i 's/,NULL,/,,/g' $f;
    sed -i 's/,NULL$/,/g' $f;
done
