import os
import subprocess
import csv
import time
import datetime
import re

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
    def __init__(self,name="",is_secure=False):
        self.name=name
        self.is_secure=is_secure

class TestConfig:
    def __init__(self,name=""):
        self.flags=[]
        self.tests=[]
        self.name = name 

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

    def add_test(self,name,is_secure):
        test=Test(name,is_secure)
        self.tests.append(test)

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

    #use this to dump flags to a flagfile
    def dump_flags(self, filename):
        with open(os.path.join(flagfile_path,filename), 'w') as file:
            lines = []
            csvwriter = csv.writer(file)
            for flag in self.flags:
                lines.append([flag.to_string()])
            csvwriter.writerows(lines) 
     
#note: party flags are constructed automatically, and can be omitted from configs
class Runner:
    def __init__(self,config,logfile=""):
        self.config=config
        self.is_host=True
        self.run_local=True     #running secure test cases locally?
        if logfile == "":
            self.logfile=os.path.join(logfile_path,datetime.datetime.now().strftime("%m-%d-%Y--%H-%M-%S"))
        else:
            self.logfile=os.path.join(logfile_path, logfile)
        self.procs=[]

    def set_logfile(self,logfile):
        self.logfile=os.path.join(logfile_path, logfile)

    def build_args(self,test):
        test_path = os.path.join(bin_path, test.name)
        args=[]
        args.append(test_path)
        for flag in self.config.flags:
            if flag.enabled:
                args.append(flag.to_string())
        return args

    def fork_test_process(self,test):
        args = self.build_args(test)
        print(subprocess.list2cmdline(args)) 
        with open(self.logfile + ".log", "a") as outfile:
            subprocess.run(args, stdout=outfile)

    def fork_background_test_process(self,test):
        args = self.build_args(test)
        print(subprocess.list2cmdline(args)) 
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

    def run_plain_test(self,test):
        print("running test " + test.name + " ")        
        self.fork_test_process(test)

    def run_secure_test_locally(self,test):
        print("running secure test " + test.name + " locally ")

        alice_flag=Flag("party","1",True)
        self.config.flags.append(alice_flag)
        self.fork_background_test_process(test)
        self.config.flags.pop()

        bob_flag=Flag("party","2",True)
        self.config.flags.append(bob_flag)
        self.fork_test_process(test)
        self.config.flags.pop()

        self.wait_for_background_processes()
        self.clean_background_processes() 

    def run_remote_test(self,test):
        print("running secure test " + test.name + " remotely ") 
        if self.is_host:
            alice_flag=Flag("party","1",True)
            self.config.flags.append(alice_flag)
            self.fork_test_process(test)
            self.config.flags.pop()
        else:
            bob_flag=Flag("party","2",True)
            self.config.flags.append(bob_flag)
            self.fork_test_process(test)
            self.config.flags.pop()

    def run_tests(self):
        for test in self.config.tests:
            if not test.is_secure:
                self.run_plain_test(test)
            elif self.run_local:
                self.run_secure_test_locally(test)
            else:
                self.run_remote_test(test)
        print("logfile: " + self.logfile + ".log")
