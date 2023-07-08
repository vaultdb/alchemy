make -j $1
killall -q $1

./bin/$1 --party=1 &
./bin/$1 --party=2
sleep 0.5
