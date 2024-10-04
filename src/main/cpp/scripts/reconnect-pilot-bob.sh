#!/bin/bash -x

SSH_PID=$(ps ax | grep 'ssh -f' | head  -n 1  | sed 's/\ .*$//g')
kill $SSH_PID

sleep 1

ssh -f -i /home/vaultdb/.ssh/catalyst -N -p 3333 -L 4444:127.0.0.1:4444  vaultdb@alice
