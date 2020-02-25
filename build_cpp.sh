#!/bin/bash
cd src/main/cpp
./configure.sh
mkdir build
cd build
cmake ..
make -j10
make install
