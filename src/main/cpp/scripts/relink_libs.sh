unlink bin/libemp-tool.dylib
unlink bin/libemp-zk.dylib

ln -s /usr/local/lib/libemp-tool.dylib  bin/
ln -s /usr/local/lib/libemp-zk.dylib  bin/

unlink libemp-tool.dylib
unlink libemp-zk.dylib
ln -s /usr/local/lib/libemp-tool.dylib  libemp-tool.dylib
ln -s /usr/local/lib/libemp-zk.dylib  libemp-zk.dylib

unlink cmake-build-debug/bin/libemp-tool.dylib
unlink cmake-build-debug/bin/libemp-zk.dylib

ln -s /usr/local/lib/libemp-tool.dylib cmake-build-debug/bin/
ln -s /usr/local/lib/libemp-zk.dylib cmake-build-debug/bin/
