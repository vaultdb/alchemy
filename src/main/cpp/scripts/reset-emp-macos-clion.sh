unlink cmake-build-debug/libemp-tool.dylib
unlink cmake-build-debug/libemp-zk.dylib

unlink cmake-build-debug/bin/libemp-tool.dylib
unlink cmake-build-debug/bin/libemp-zk.dylib

ln -s /usr/local/lib/libemp-zk.dylib cmake-build-debug/
ln -s /usr/local/lib/libemp-tool.dylib cmake-build-debug/

ln -s /usr/local/lib/libemp-zk.dylib cmake-build-debug/bin/
ln -s /usr/local/lib/libemp-tool.dylib cmake-build-debug/bin/
