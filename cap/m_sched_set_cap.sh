#!/usr/bin/env bash

set -x
(
    cp sched_set_caps /tmp/sched_set_caps
    cd /tmp

    sudo setcap 'cap_sys_nice=p' sched_set_caps
    getcap sched_set_caps

    sleep 1 &
    pid=$!
    sleep 1 &
    pid2=$!
    ./sched_set_caps r 3 $pid $pid2
    ps -o"pid,cls,pri,comm"
    wait
)

exit 0
