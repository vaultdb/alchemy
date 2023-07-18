#make -j $1
killall -q $1

./bin/$1 --flagfile=flagfiles/alice.flags --storage="column" &
./bin/$1 --flagfile=flagfiles/bob.flags --storage="column"
sleep 0.5
