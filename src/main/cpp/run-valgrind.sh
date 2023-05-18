#args: test name and args
#e.g.,
#bash run-valgrind.sh bin/value_expression_test
docker run -ti -v $PWD:/test memory-test:0.1 bash -c "cmake . && make -j5  && valgrind --leak-check=full ./$@"
