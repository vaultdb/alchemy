rm -rf emp-*
git clone https://github.com/emp-toolkit/emp-readme.git
python3 ./emp-readme/scripts/install.py -install -tool -ot -sh2pc

cd emp-tool
cmake -DENABLE_FLOAT=On .
make clean all

sudo make install


cd ..
git clone https://github.com/emp-toolkit/emp-zk.git
cd emp-zk

cmake -DENABLE_FLOAT=On .
make clean all

sudo make install
