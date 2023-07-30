#!/usr/python

import argparse
import re
from datetime import datetime

parser = argparse.ArgumentParser()
parser.add_argument('-input', type=str, help='input file', required=True)
parser.add_argument('-output', type=str, help='input file', required=True) #writes to csv
parser.add_argument('--branch', type=str, help='source branch in repo',  default='emp-operators')


args = parser.parse_args()
input_file=args.input
output_file=args.output
branch=args.branch


if(re.search('log/all-tests-alice', input_file)):
    party='alice'
elif(re.search('log/all-tests-bob', input_file)):
    party='bob'
else:
    print("party name not found!")
    exit(-1)

storage_model='row'
if(re.search('-column-store-', input_file)):
    storage_model='column'

timestamp_str = re.findall('-(\d+).log', input_file)[0]
datetime_obj = datetime.strptime(timestamp_str, '%Y%m%d%H%M%S')


read_file = open(input_file, "r")
write_file = open(output_file, "w")

for line in read_file:
    if(re.search('ms\)', line)):
        # print(line)
        test_suite = re.findall(']\ ([a-z]+)\.', line,  flags=re.IGNORECASE)[0]
        mpc='f'
        if(re.search('Secure|Emp', test_suite) or re.search('FullyOptimized', branch)):
            mpc='t'


        test_name=re.findall('\.(.*?)\ ', line,  flags=re.IGNORECASE)[0]
        runtime=re.findall('(\d+) ms\)', line)[0]

        lst = [test_suite, test_name, mpc, storage_model, party, input_file, branch, datetime_obj, runtime]
        output = ','.join(map(str, lst))
    #    print(output)
        write_file.write(output + '\n')

read_file.close()
write_file.close()



