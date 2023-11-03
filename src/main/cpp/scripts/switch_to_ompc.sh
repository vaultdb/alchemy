cd /usr/local/include
sudo rm -rf emp-tool/
sudo  cp -r emp-ompc/* .

cd ../lib
sudo rm libemp-*
sudo cp emp-ompc/* .