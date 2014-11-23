#!/usr/bin/env bash

f='/tmp/fcntl_locking_spec'

./fcntl_locking_spec_a $f &
pid=$?
sleep 30

time ./fcntl_locking_spec_b $f 0
time ./fcntl_locking_spec_b $f 10000
time ./fcntl_locking_spec_b $f 20000
time ./fcntl_locking_spec_b $f 30000
time ./fcntl_locking_spec_b $f 40000

kill $?
