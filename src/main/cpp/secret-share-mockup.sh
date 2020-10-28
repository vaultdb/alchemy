#secret share tool
clang++ -std=c++11  secret_share/secret_share_ints.cpp -o bin/secret_share_ints
#reveal tool for verification
clang++  -pthread -Wall -march=native -maes -mrdseed -funroll-loops -std=c++11 -O3 -DNDEBUG   -I/usr/local/include \
	  /usr/local/opt/openssl/lib/libssl.dylib /usr/local/opt/openssl/lib/libcrypto.dylib /usr/local/lib/libemp-tool.dylib \
	  secret_share/reveal_secret_shares.cpp  -o bin/reveal_secret_shares


./bin/secret_share_ints  #writes alice.enc and bob.enc to local directory

./bin/reveal_secret_shares 1 54321 &
./bin/reveal_secret_shares 2 54321


