#!/bin/bash
SITE=nm

if [ "$#" -eq 1 ]; then
    SITE=$1
fi

echo 'Site ID: '$SITE


#run this before loading the data
for f in $(ls pilot/input/$SITE/*.csv);
do
    dos2unix $f
    sed -i 's/,NULL,/,,/g' $f;
    sed -i 's/,NULL$/,/g' $f;
done
