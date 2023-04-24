find . -iname CMakeCache.txt | xargs rm
find . -iname CMakeFiles | xargs rm -r
rm -rf cmake-build-debug
rm -rf cmake-build-release
rm lib/*
rm bin/*
rm test/bin/*
rm -rf  libvault*
#cmake .
#make clean all -j4
