find . -iname CMakeCache.txt | xargs rm
find . -iname CMakeFiles | xargs rm -r
rm -rf cmake-build-*
rm -rf lib/*
rm -rf  bin/*
rm -rf test/bin/*
rm -rf  libvault*
rm -rf build/*
rm -rf _deps/google*
#cmake .
#make clean all -j
