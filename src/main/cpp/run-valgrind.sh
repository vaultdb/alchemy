#args: test name and args
#e.g.,
#bash run-valgrind.sh bin/value_expression_test $PARTY
TEST=$1
PARTY=$2
valgrind --leak-check=full     \
         --show-leak-kinds=all   \
         --track-origins=yes     \
          --log-file=log/valgrind-$TEST-p$PARTY.txt ./bin/$TEST --party=$PARTY


# plaintext
# valgrind --leak-check=full     \
#         --show-leak-kinds=all   \
#         --track-origins=yes     \
#          --log-file=log/valgrind-$TEST.log ./bin/$TEST