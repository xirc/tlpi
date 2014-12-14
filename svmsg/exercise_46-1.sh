#!/usr/bin/env bash

# remove all message queue
ipcs -q | awk '{print $2}' | xargs -I% ipcrm -q % 2>/dev/null

# tests
id=$(./svmsg_create -p)
./svmsg_send $id 10 'hello'
./svmsg_send $id 20 'world'
./svmsg_send $id 30 'foo'
./svmsg_receive $id | grep 'hello' || echo 'FAIL'
./svmsg_receive -t 30 $id | grep 'foo' || echo 'FAIL'
./svmsg_receive $id | grep 'world' || echo 'FAIL'

./svmsg_send $id 10 'hello'
./svmsg_send $id 20 'world'
./svmsg_send $id 30 'foo'
./svmsg_receive -xt 10 $id | grep 'world' || echo 'FAIL'
./svmsg_receive -t 10 $id 3 && echo 'FAIL'
./svmsg_receive -e -t 10 $id 3 | grep 'hel' || echo 'FAIL'
while ./svmsg_receive -n $id
do
:
done 2>/dev/null

echo 'svmsg_receive -n ...'
./svmsg_send $id 20 'world'
./svmsg_send $id 10 'hello'
./svmsg_send $id 30 'foo'
while ./svmsg_receive -n $id
do
:
done 2>/dev/null

echo 'svmsg_receive -n -t -20 ...'
./svmsg_send $id 20 'world'
./svmsg_send $id 30 'foo'
./svmsg_send $id 10 'hello'
while ./svmsg_receive -n -t -20 $id
do
:
done 2>/dev/null
while ./svmsg_receive -n $id
do
:
done >/dev/null 2>/dev/null

./svmsg_create -f ./svmsg_create 2>/dev/null && echo 'FAIL'
./svmsg_create -cxf ./svmsg_create >/dev/null || echo 'FAIL'
./svmsg_create -cf ./svmsg_create >/dev/null || echo 'FAIL'
./svmsg_create -f ./svmsg_create >/dev/null || echo 'FAIL'
./svmsg_create -cxf ./svmsg_create 2>/dev/null && echo 'FAIL'
./svmsg_create -k $id 2>/dev/null && echo 'FAIL'

key=`ipcs -q | grep $id | awk '{print $1}'`
./svmsg_create -k $key >/dev/null || echo 'FAIL'

# remove all message queue
ipcs -q | awk '{print $2}' | xargs -I% ipcrm -q % 2>/dev/null
