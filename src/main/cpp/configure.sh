#!/bin/bash

BASEDIR=`pwd`
echo $BASEDIR
echo "Setting up for vaultdb"

if [ ! -d "build" ]; then
	mkdir build
fi

if [ ! -d "external" ]; then
	mkdir external
fi

if [ ! -d "lib" ]; then
	mkdir lib
fi



#gflags installation
cd $BASEDIR/external
if [ ! -d "gflags" ]; then
	git clone https://github.com/gflags/gflags.git
	cd gflags
	mkdir build && cd build
	cmake -DCMAKE_INSTALL_PREFIX=$BASEDIR/lib ..
	make -j10
	make install
	cd $BASEDIR/external
fi


#libpqxx installation
cd $BASEDIR/external
if [ ! -d "libpqxx" ]; then
	git clone --branch 6.2.5 https://github.com/jtv/libpqxx.git
	cd libpqxx
	./configure --prefix=$BASEDIR/lib/ --disable-documentation
	make -j10
	make install
fi

cd $BASEDIR/external
if [ ! -d "emp-tool" ]; then
	git clone https://github.com/emp-toolkit/emp-tool.git
	cd emp-tool
	mkdir build && cd build
	cmake -DCMAKE_INSTALL_PREFIX=$BASEDIR/lib ..
	make -j10
	make install
fi

cd $BASEDIR/external
if [ ! -d "emp-ot" ]; then
	git clone https://github.com/emp-toolkit/emp-ot.git
	cd emp-ot
	mkdir build && cd build
	cmake -DCMAKE_PREFIX_PATH=$BASEDIR/lib -DCMAKE_INCLUDE_PATH=$BASEDIR/lib/include -DCMAKE_INSTALL_PREFIX=$BASEDIR/lib ..
	make -j10
	make install
fi

cd $BASEDIR/external
if [ ! -d "emp-sh2pc" ]; then
	git clone https://github.com/emp-toolkit/emp-sh2pc.git
	cd emp-sh2pc
	mkdir build && cd build
	cmake -DCMAKE_PREFIX_PATH=$BASEDIR/lib -DCMAKE_INCLUDE_PATH=$BASEDIR/lib/include -DCMAKE_INSTALL_PREFIX=$BASEDIR/lib ..
	make -j10
	make install
fi



cd $BASEDIR
#cd build
#cmake ..
#make -j10 vaultdb
#make -j10
#ctest 



