from runner import TestConfig
from runner import Runner
import sys
import argparse 

def run_tests(regex_=".*",label_="UnitTest",regex_exclude_="a^",label_exclude_="ZkTest",filter_="*"):
    config = TestConfig()
    config.add_from_ctest(regex=regex_,label=label_,regex_exclude=regex_exclude_,label_exclude=label_exclude_)
    for test_suite in config.test_suites:
        test_suite.add_flag("log_level", "2", True)
        test_suite.edit_flag("filter",filter_)
        test_suite.edit_flag("unioned_db","tpch_unioned_600")
    runner = Runner(config)
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

args=parser.parse_args()

run_tests(regex_=args.regex, label_=args.label, regex_exclude_=args.regex_exclude, label_exclude_=args.label_exclude,filter_=args.filter)
