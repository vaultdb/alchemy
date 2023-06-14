#make -j $1
killall -q $1

./bin/$1 --party=1 --storage="column" &
./bin/$1 --party=2 --storage="column"
sleep 0.5
