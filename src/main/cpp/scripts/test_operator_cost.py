import runner
import os
import subprocess

dir_path = os.path.dirname(os.path.realpath(__file__))
bin_path = os.path.join(dir_path, 'bin')
flagfile_path = os.path.join(dir_path, "flagfiles")

stride=50
limit=1500

args = []

for i in range(stride, limit+1, stride):
    subprocess.run(["python3", os.path.join(dir_path, "run_unit_tests.py"), "--regex", "SecureNestedLoopAggregateTest", "--filter", "*.tpch_q9", "--run_local", "False", "--is_host", "True", "--cutoff", str(i)])
