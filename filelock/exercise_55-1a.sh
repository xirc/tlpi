#!/usr/bin/env bash

f=$(mktemp)

while :
do
    ./t_flock $f s 5 &
    sleep 3
done &
pid=$?
trap "unlink $f; kill $pid" EXIT

# ./t_flock $f x はロックを獲得できない
sleep 1
./t_flock $f x
