#!/bin/bash -x

SSH_PID=$(ps ax | grep 'ssh -f' | head  -n 1  | sed 's/\ .*$//g')
kill $SSH_PID

sleep 1

#ssh -f -i /home/vaultdb/.ssh/catalyst -N -p 3333 -L 4444:127.0.0.1:4444  vaultdb@alice
#
# add a hop for alliance
ssh -f -N -p 3333 -J 10.146.0.4 -L 4444:127.0.0.1:4444  vaultdb@165.124.123.122
