#!/bin/bash -x

cd /usr/local/include
sudo rm -rf emp-tool/
sudo  cp -r emp-2pc/* .

cd ../lib
sudo rm libemp-*
sudo cp emp-2pc/* .

#for macos
unlink bin/libemp-tool.dylib
ln -s /usr/local/lib/libemp-tool.dylib bin/

unlink bin/libemp-zk.dylib
ln -s /usr/local/lib/libemp-zk.dylib bin/

unlink cmake-build-debug-local/libemp-tool.dylib
unlink cmake-build-debug-local/libemp-zk.dylib
ln -s /usr/local/lib/libemp-zk.dylib cmake-build-debug-local/
ln -s /usr/local/lib/libemp-tool.dylib cmake-build-debug-local/