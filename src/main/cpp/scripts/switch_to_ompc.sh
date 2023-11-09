cd /usr/local/include
sudo rm -rf emp-tool/ emp-ot/ emp-sh2pc  emp-zk emp-zk-set
sudo  cp -r emp-ompc/* .

cd ../lib
sudo rm libemp-*
sudo cp emp-ompc/* .