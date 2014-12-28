#!/usr/bin/env bash

f=$(mktemp)
trap "unlink $f" EXIT


transaction()
{
./m_lockfile $f.lock

for i in `seq 1 3`
do
if read a <$f ; then
  b=`expr $a + 1`
  echo $b >$f
fi
done

rm -f $f.lock
}


echo 0 >$f
for i in `seq 1 3`
do
  transaction &
done
wait
cat $f

exit 0
