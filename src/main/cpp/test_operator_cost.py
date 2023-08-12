import runner
import os

dir_path = os.path.dirname(os.path.realpath(__file__))
bin_path = os.path.join(dir_path, 'bin')
flagfile_path = os.path.join(dir_path, "flagfiles")

config = runner.TestConfig("scaleout_test")
config.add_from_ctest(regex = "SecureNestedLoopAggregateTest.tpch_q5")

test_suite = config.test_suites[0]

test_suite.read_from_flagfile("flagfiles/plain.flags")
test_suite.add_flag("logfile", "log/nla_scaleout.log", True)
test_suite.edit_flag("filter", "*.tpch_q5")

test_runner = runner.Runner(config)

stride=50
limit=1500

for i in range(stride, limit+1, stride):
    test_suite.edit_flag("cutoff",str(i))
    test_runner.run_tests()

config.test_suites.pop()
config.add_from_ctest(regex = "SecureGroupByAggregateTest.tpch_q5")

test_suite = config.test_suites[0]

test_suite.read_from_flagfile("flagfiles/plain.flags")
test_suite.add_flag("logfile", "log/sma_scaleout.log", True)
test_suite.edit_flag("filter", "*.tpch_q5")

test_runner = runner.Runner(config)

stride=50
limit=1500

for i in range(stride, limit+1, stride):
    test_suite.edit_flag("cutoff",str(i))
    test_runner.run_tests()

config.test_suites.pop()


config.add_from_ctest(regex = "SecureKeyedJoinTest.test_tpch_q3_customer_orders")

test_suite = config.test_suites[0]

test_suite.read_from_flagfile("flagfiles/plain.flags")
test_suite.add_flag("logfile", "log/keyed_join_scaleout.log", True)
test_suite.edit_flag("filter", "*.test_tpch_q3_customer_orders")

for i in range(stride, limit+1, stride):
    test_suite.edit_flag("cutoff",str(i))
    test_runner.run_tests()

config.test_suites.pop()
config.add_from_ctest(regex = "SecureSortMergeJoinTest.test_tpch_q3_customer_orders")
test_suite = config.test_suites[0]

test_suite.read_from_flagfile("flagfiles/plain.flags")
test_suite.add_flag("logfile", "log/smj_scaleout.log", True)
test_suite.edit_flag("filter", "*.test_tpch_q3_customer_orders")

for i in range(stride, limit+1, stride):
    test_suite.edit_flag("cutoff",str(i))
    test_runner.run_tests()
