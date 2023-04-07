#!/bin/bash 

rm -rf emp-* install.py
wget https://raw.githubusercontent.com/emp-toolkit/emp-readme/master/scripts/install.py
python3 install.py --deps --tool --ot --sh2pc --zk
#git clone https://github.com/emp-toolkit/emp-readme.git
#python3 ./emp-readme/scripts/install.py -install -tool -ot -sh2pc

#cd emp-tool
#cmake -DENABLE_FLOAT=On .
#make clean all

#sudo make install


#cd ..
#git clone https://github.com/emp-toolkit/emp-zk.git
#cd emp-zk

#cmake -DENABLE_FLOAT=On .
#make clean all

#sudo make install
