#!/usr/bin/env bash

f=$(mktemp)
trap "unlink $f" EXIT

./t_flock $f x 5 &

sleep 1
./t_flock $f s 1 &
./t_flock $f x 1 &
./t_flock $f x 1 &
./t_flock $f s 1 &
./t_flock $f s 1 &

# ロックを獲得するプロセスを決定する規則は、
# プロセスのスケジューリングに依存する。
wait
