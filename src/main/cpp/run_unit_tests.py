from runner import TestConfig
from runner import Runner
import sys
import argparse 

def run_tests(regex_="*",label_="UnitTest",regex_exclude_="a^",label_exclude_="ZkTest"):
    config = TestConfig()
    config.add_from_ctest(regex=regex_,label=label_,regex_exclude=regex_exclude_,label_exclude=label_exclude_)
    for test_suite in config.test_suites:
        test_suite.add_flag("log_level", "2", True)
    runner = Runner(config)
    runner.run_tests()

parser = argparse.ArgumentParser()

parser.add_argument("--regex", type=str,
                    default=".*",
                    help="Run only the tests with names matching this regex")
parser.add_argument("--label", type=str, 
                    default="UnitTest",
                    help="Run only the tests having this label")
parser.add_argument("--regex_exclude",type=str,
                    default="^$",
                    help="Exclude tests with names matching this regex")
parser.add_argument("--label_exclude", type=str,
                    default="ZkTest",
                    help="Exclude the tests having this label")

args=parser.parse_args()

run_tests(regex_=args.regex, label_=args.label, regex_exclude_=args.regex_exclude, label_exclude_=args.label_exclude)
