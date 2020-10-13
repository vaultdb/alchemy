git clone https://github.com/emp-toolkit/emp-readme.git
python ./emp-readme/scripts/install.py -install -tool -ot -sh2pc

cd emp-tool
cmake -DENABLE_FLOAT=On .
make clean all
sudo make install
