#!/usr/bin/env bash

./t_clock_nanosleep 10 0 &
PID=$!
while `kill -0 $PID`
do
  kill -INT $PID
done
