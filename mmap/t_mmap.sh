#!/usr/bin/env bash

set -x
f=$(mktemp)
dd if=/dev/zero of=$f bs=1 count=1024
hexdump -c $f

./t_mmap $f hello
hexdump -c $f

./t_mmap $f goodbye
hexdump -c $f

./t_mmap $f too_loooooooong && echo 'UNEXPECTED'
