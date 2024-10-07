#!/bin/bash -x

SSH_PID=$(ps -ef | grep 'ssh -f' | grep -v grep | awk '{print $2}')
kill $SSH_PID

sleep 1

#ssh -f -i /home/vaultdb/.ssh/catalyst -N -p 3333 -L 4444:127.0.0.1:4444  vaultdb@alice
#
# add a hop for alliance
ssh -f -N -i /home/vaultdb/.ssh/catalyst -p 3333 -J 10.146.0.4 -L 4444:127.0.0.1:4444  vaultdb@165.124.123.122
