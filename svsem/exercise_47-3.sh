#!/usr/bin/env bash

set -x

semid=$(./svsem_create -p 1)
trap "./svsem_rm $semid" EXIT
./svsem_setall $semid 1

./svsem_op $semid 0+1 &
echo "LAST SEMOP PID=$!"
./svsem_mon $semid

./svsem_op $semid 0+1u &
echo "LAST SEMOPu PID=$!"
./svsem_mon $semid

# SEMPID is changed even if we use SEM_UNDO
