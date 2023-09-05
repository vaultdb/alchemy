from runner import TestConfig
from runner import Runner
import sys
import argparse 

def run_tests(regex_=".*",label_="UnitTest",regex_exclude_="a^",label_exclude_="ZkTest",filter_="*",run_local_="True",is_host_="True",alice_host_="127.0.0.1", cutoff_=-2):
    config = TestConfig()
    config.add_from_ctest(regex=regex_,label=label_,regex_exclude=regex_exclude_,label_exclude=label_exclude_)
    for test_suite in config.test_suites:
        test_suite.add_flag("log_level", "2", True)
        test_suite.edit_flag("filter",filter_)
        test_suite.edit_flag("unioned_db","tpch_unioned_1500")
        test_suite.edit_flag("alice_db","tpch_alice_1500")
        test_suite.edit_flag("bob_db","tpch_bob_1500")
        test_suite.edit_flag("alice_host",alice_host_)
        if cutoff_ != -2:
            test_suite.edit_flag("cutoff",str(cutoff_))
    runner = Runner(config,is_host= (is_host_ == "True"))
    runner.run_local= (run_local_ == "True")
    runner.run_tests()

parser = argparse.ArgumentParser()

parser.add_argument("--regex", type=str,
                    default=".*",
                    help="Run only the test suites with names matching this regex")
parser.add_argument("--label", type=str, 
                    default="UnitTest",
                    help="Run only the test suites having this label")
parser.add_argument("--regex_exclude",type=str,
                    default="^$",
                    help="Exclude test suites with names matching this regex")
parser.add_argument("--label_exclude", type=str,
                    default="ZkTest",
                    help="Exclude the test suites having this label")
parser.add_argument("--filter", type=str,
                    default="*",
                    help="Run only the tests matching this filter")
parser.add_argument("--run_local",type=str,
                    default="True",
                    choices=["True","False"],
                    help="If true, run alice and bob processes on this host")
parser.add_argument("--alice_host", type=str,
                    default="127.0.0.1",
                    help="If running as bob: IP address of alice. If running as alice: 127.0.0.1")
parser.add_argument("--is_host", type=str,
                    default="True",
                    choices=["True","False"],
                    help="True: run as alice. False: run as bob")
parser.add_argument("--cutoff", type=int,
                    default=-2,
                    help="Limit clause")

args=parser.parse_args()

run_tests(regex_=args.regex, label_=args.label, regex_exclude_=args.regex_exclude, label_exclude_=args.label_exclude,filter_=args.filter,run_local_=args.run_local,is_host_=args.is_host,alice_host_=args.alice_host, cutoff_=args.cutoff)
