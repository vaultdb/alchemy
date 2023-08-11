import runner

config = runner.TestConfig()
config.add_from_ctest(label="UnitTest",label_exclude="ZkTest")
runner = runner.Runner(config)
runner.run_tests()
