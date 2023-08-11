import runner
import os

dir_path = os.path.dirname(os.path.realpath(__file__))
bin_path = os.path.join(dir_path, 'bin')
flagfile_path = os.path.join(dir_path, "flagfiles")

config = runner.TestConfig("scaleout_keyed_join")
config.add_from_ctest(regex = "SecureKeyedJoinTest.test_tpch_q3_customer_orders")

test_suite = config.test_suites[0]
test_suite.read_from_flagfile(os.path.join(flagfile_path, "plain.flags"))

test_runner = runner.Runner(config, "keyed_join_scaleout")

stride=50
limit=1500

for i in range(stride, limit+1, stride):
    test_suite.edit_flag("cutoff",str(i))
    test_runner.run_tests()

test_runner.set_logfile("sort_merge_join_scaleout")
config.test_suites.pop()
config.add_from_ctest(regex = "SecureSortMergeJoinTest.test_tpch_q3_customer_orders")
test_suite = config.test_suites[0]
test_suite.read_from_flagfile(os.path.join(flagfile_path, "plain.flags"))

for i in range(stride, limit+1, stride):
    test_suite.edit_flag("cutoff",str(i))
    test_runner.run_tests()
