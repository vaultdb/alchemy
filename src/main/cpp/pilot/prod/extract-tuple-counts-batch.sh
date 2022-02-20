#!/bin/bash

#usage: bash pilot/prod/extract-tuple-counts-batch.sh <filename>

grep -i read $1  | sed 's/^.*Reading\ in\ //g' | sed 's/^.*read\ in\ //g' | grep -v epoch | sed 's/\ .*$//g' | tr '\n' '+' | sed 's/\+$//' 
