import os
import subprocess
import csv
import time
import datetime
import re
import json

dir_path = os.path.dirname(os.path.realpath(__file__))
bin_path = os.path.join(dir_path, 'bin')
flagfile_path = os.path.join(dir_path, "flagfiles")
logfile_path = os.path.join(dir_path, "log")

class Flag:
    def __init__(self,name="",value="",enabled=False):
        self.name=name
        self.value=value
        self.enabled=enabled

    def to_string(self):
        return "--" + self.name + "=" + self.value

class Test:
    def __init__(self,name=""):
        self.name=name
        
class TestSuite:
    def __init__(self,name,exe_path,is_secure=False):
        self.name = name
        self.exe_path = exe_path
        self.is_secure=is_secure
        self.flags=[]
        self.tests=[]

    #NB: Flag names must be unique
    def add_flag(self,name,value,enabled):
        flag=Flag(name,value,enabled)
        self.flags.append(flag)

    def find_flag_by_name(self,name):
        for flag in self.flags:
            if flag.name == name:
                return flag
        default = Flag()
        return default

    def toggle_flag(self,name):
        flag = self.find_flag_by_name(name)
        flag.enabled = not flag.enabled

    def delete_flag(self,name):
        flag = self.find_flag_by_name(name)
        self.flags.remove(flag)

    def edit_flag(self,name,value):
        flag = self.find_flag_by_name(name)
        flag.value = value

    def parse_flag(self, flag_string):
        parts=flag_string.split("=")
        flag=Flag(parts[0][2:],parts[1].strip(),True)
        return flag

    def read_from_flagfile(self, path):
        with open(path) as flagfile:
            lines = flagfile.readlines()
            for line in lines:                
                if line[0] == "#":
                    pass
                else:
                    flag = self.parse_flag(line)
                    if flag.name == "flagfile":
                        self.read_from_flagfile(flag.value)
                    else:
                        self.flags.append(flag)

    def read_from_gflags(self):
        args = [self.exe_path,"--helpshort"]
        proc = subprocess.run(args, capture_output=True, text=True)
        raw = proc.stdout.split(".cpp:")
        for flag_string in raw[1:]:
            lines = flag_string.split("    -")
            for line in lines:
                if len(line.strip()) > 0:
                    flag_name = line[:line.index("(")-1] 
                    flag_value = line[line.index("default:")+9:line.rindex("\n")]
                    flag_value=flag_value.replace('"', '')
                    self.add_flag(flag_name, flag_value, True)

    #use this to dump flags to a flagfile
    def dump_flags(self, filename):
        with open(os.path.join(flagfile_path,filename), 'w') as file:
            lines = []
            csvwriter = csv.writer(file)
            for flag in self.flags:
                lines.append([flag.to_string()])
            csvwriter.writerows(lines) 

    def add_test(self,name):
        test=Test(name)
        self.tests.append(test)

class TestConfig:
    def __init__(self,name=""):
        self.test_suites=[]
        self.name = name 

    #regex limits to tests with matching names
    def add_from_ctest(self,regex="",label="",regex_exclude="",label_exclude=""):
        args = ["ctest", "--show-only=json-v1"]
        if not regex == "":
            args.append("-R")
            args.append(regex)
        if not label == "":
            args.append("-L")
            args.append(label)
        if not regex_exclude == "":
            args.append("-E")
            args.append(regex_exclude)
        if not label_exclude == "":
            args.append("-LE")
            args.append(label_exclude)
        print("Setting ctest config: " + " ".join(args))

        raw = subprocess.check_output(args)
        tests_json = json.loads(raw)["tests"]
        prev_suite_name = ""
        for test in tests_json:
            full_name = test["name"]
            suite_name = full_name[:full_name.index(".")]
            if not prev_suite_name == suite_name:
                test_suite = TestSuite(suite_name,test["command"][0])
                test_suite.read_from_gflags()
                self.test_suites.append(test_suite)
                suite_properties = test["properties"]
                for property in suite_properties:
                    if property["name"] == "LABELS":
                        test_labels = property["value"]
                        if "EmpTest" in test_labels:
                            test_suite.is_secure = True
                        else:
                            test_suite.is_secure = False

                prev_suite_name = suite_name

            current_test_suite = self.test_suites[-1]
            test_name = full_name[full_name.index(".")+1:]
            current_test_suite.add_test(test_name)
     
#note: party flags are constructed automatically, and can be omitted from configs
class Runner:
    def __init__(self,config,logfile="",verbose=True):
        self.config=config
        self.is_host=True
        self.run_local=True     #running secure test cases locally?
        if logfile == "":
            self.logfile = ""
        else:
            self.logfile=os.path.join(logfile_path, logfile)
        self.verbose = verbose
        self.procs=[]

    def set_logfile(self,logfile):
        self.logfile=os.path.join(logfile_path, logfile)

    def build_args(self,test_suite):
        test_exe_path = os.path.join(bin_path, test_suite.exe_path)
        args=[]
        args.append(test_exe_path)
        for flag in test_suite.flags:
            if flag.enabled:
                args.append(flag.to_string())
        print(subprocess.list2cmdline(args))
        return args

    def fork_test_process(self,test_suite):
        args = self.build_args(test_suite)
        if self.logfile == "":
            subprocess.run(args)
        else:
            with open(self.logfile + ".log", "a") as outfile:            
               subprocess.run(args, stdout=outfile)            

    def fork_background_test_process(self,test_suite):
        args = self.build_args(test_suite)
        if self.logfile == "":
            proc = subprocess.Popen(args)
            self.procs.append(proc) 
        else:            
            with open(self.logfile + ".log", "a") as outfile:            
                proc = subprocess.Popen(args, stdout=outfile)
                self.procs.append(proc)

    def background_processes_done(self):
        for proc in self.procs:
            if proc.poll == None:
                return False
        return True

    def wait_for_background_processes(self):
        while not self.background_processes_done():
            time.sleep(1)

    def clean_background_processes(self):
        for proc in self.procs:
            proc.terminate()
            self.procs.remove(proc)

    def run_plain_test(self,test_suite):
        print("running plain test suite " + test_suite.name + " ")        
        self.fork_test_process(test_suite)

    def run_secure_test_locally(self,test_suite):
        print("running secure test suite " + test_suite.name + " locally ")

        alice_flag=Flag("party","1",True)
        test_suite.flags.append(alice_flag)
        self.fork_background_test_process(test_suite)
        test_suite.flags.pop()

        bob_flag=Flag("party","2",True)
        test_suite.flags.append(bob_flag)
        self.fork_test_process(test_suite)
        test_suite.flags.pop()

        self.wait_for_background_processes()
        self.clean_background_processes() 

    def run_remote_test(self,test_suite):
        print("running secure test " + test_suite.name + " remotely ") 
        if self.is_host:
            alice_flag=Flag("party","1",True)
            test_suite.flags.append(alice_flag)
            self.fork_test_process(test_suite)
            test_suite.flags.pop()
        else:
            bob_flag=Flag("party","2",True)
            test_suite.flags.append(bob_flag)
            self.fork_test_process(test_suite)
            test_suite.flags.pop()

    def run_tests(self):
        for test_suite in self.config.test_suites:
            if not test_suite.is_secure:
                self.run_plain_test(test_suite)
            elif self.run_local:
                self.run_secure_test_locally(test_suite)
            else:
                self.run_remote_test(test_suite)
        if not self.logfile == "":
            print("logfile: " + self.logfile + ".log")

config = TestConfig()
config.add_from_ctest(label="UnitTest",label_exclude="ZkTest")
runner = Runner(config)
runner.run_tests()
