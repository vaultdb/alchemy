unlink cmake-build-debug/libemp-tool.dylib
unlink cmake-build-debug/libemp-zk.dylib

unlink cmake-build-debug/bin/libemp-tool.dylib
unlink cmake-build-debug/bin/libemp-zk.dylib

unlink bin/libemp-tool.dylib
unlink bin/libemp-zk.dylib

unlink libemp-tool.dylib
unlink libemp-zk.dylib

mkdir -p cmake-build-debug/bin
mkdir bin

ln -s /usr/local/lib/libemp-zk.dylib cmake-build-debug/
ln -s /usr/local/lib/libemp-tool.dylib cmake-build-debug/

ln -s /usr/local/lib/libemp-zk.dylib cmake-build-debug/bin/
ln -s /usr/local/lib/libemp-tool.dylib cmake-build-debug/bin/


ln -s /usr/local/lib/libemp-zk.dylib bin/
ln -s /usr/local/lib/libemp-tool.dylib bin/

ln -s /usr/local/lib/libemp-zk.dylib libemp-zk.dylib
ln -s /usr/local/lib/libemp-tool.dylib libemp-tool.dylib