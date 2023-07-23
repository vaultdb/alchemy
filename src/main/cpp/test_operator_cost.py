import runner
import os

dir_path = os.path.dirname(os.path.realpath(__file__))
bin_path = os.path.join(dir_path, 'bin')
flagfile_path = os.path.join(dir_path, "flagfiles")

config = runner.TestConfig("scaleout_nla")
config.read_from_flagfile(os.path.join(flagfile_path, "plain.flags"))

config.add_test(os.path.join(bin_path, "secure_nested_loop_aggregate_test"), True)

test_runner = runner.Runner(config, "nla_tpch_q1")

stride=60000
limit=60000

for i in range(stride, limit+1, stride):
    config.edit_flag("cutoff",str(i))
    test_runner.run_tests()

test_runner.set_logfile("sma_tpch_q1_with_sort")
config.tests.pop()
config.add_test(os.path.join(bin_path, "secure_group_by_aggregate_test"), True)
for i in range(stride, limit+1, stride):
    config.edit_flag("cutoff",str(i))
    test_runner.run_tests()
