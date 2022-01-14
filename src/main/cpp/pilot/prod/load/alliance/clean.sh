#!/bin/bash

#clean up the file locations
cd pilot/input/alliance
rm *.csv
unzip vaultdb-data.zip

#measure 1 has data from 2015..2020 - so does measure 2 - maybe they are disjoint populations
mv VaultDB_Measure1.csv population-labels-1.csv
mv VaultDB_Measure2.csv population-labels-2.csv
mv VaultDB_Demographics.csv demographics.csv
rm VaultDB_Tokens.csv



