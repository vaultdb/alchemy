git clone https://github.com/emp-toolkit/emp-readme.git
python3 ./emp-readme/scripts/install.py -install -tool -ot -sh2pc

cd emp-tool
cmake -DENABLE_FLOAT=On .
make clean all
make install
