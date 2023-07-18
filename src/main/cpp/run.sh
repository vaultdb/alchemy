#make -j $1
killall -q $1

./bin/$1 --flagfile=flagfiles/alice.flags &

sleep 0.5
./bin/$1 --flagfile=flagfiles/bob.flags
