#!/usr/bin/env bash

#set -x

./test_become_daemon
ps -C test_become_daemon -o'pid ppid pgid sid tty command'
cat <<EOT
  ppid  = 1
  pgid != pid
  sid  != pid
  tty   = ??
EOT
kill $(ps -C test_become_daemon -o'pid' --no-headers)

exit 0
