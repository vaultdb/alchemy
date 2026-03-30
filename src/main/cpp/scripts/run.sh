make -j4 $1
#killall -q $1
sleep 0.5

./bin/$1 --flagfile=flagfiles/alice.flags  2>&1 | tee  log/alice.log &

sleep 0.5
./bin/$1 --flagfile=flagfiles/bob.flags 2>&1  > log/bob.log
sleep 0.5
