#!/usr/bin/env bash

set -x

semid=$(./svsem_create -p 2)
trap "./svsem_rm $semid" EXIT

./svsem_setall $semid 1 0
./svsem_mon $semid

# SEMAPHORE < 0: 1, 1: 0 >
./svsem_op $semid 0-1 1-1 &
sleep 1
./svsem_op $semid 0=0 1+1
./svsem_mon $semid

# SEMAPHORE < 0: 0, 1: 0 >
./svsem_op $semid 0-1 1-1 &
sleep 1
./svsem_op $semid 0+2 1+2
./svsem_mon $semid

# SEMAPHORE < 0: 1, 1: 1 >
./svsem_op $semid 0=0 1=0,0-1 1-1 &
sleep 1
./svsem_op $semid 0-1 1-1,0+1 1+1
./svsem_mon $semid

# SEMAPHORE < 0: 0, 0: 0 >
