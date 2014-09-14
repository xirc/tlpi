# exercise 35-2

sudo cp ./rtsched /tmp
cd /tmp
sudo chown root rtsched
sudo chgrp root rtsched
sudo chmod u+s rtsched

tmp=$(mktemp)
./rtsched f 1 echo SCHED_FIFO >> $tmp
./rtsched r 1 echo SCHED_RR >> $tmp
./rtsched o 0 echo SCHED_OTHER >> $tmp
echo 'SCHED_FIFO
SCHED_RR
SCHED_OTHER' | cmp -s $tmp -
rc=$?
rm $tmp
if [[ $rc -ne 0 ]]
then
  echo 'FAIL: program may not execute correctly'
fi

# check command does not have priviledge
./rtsched o 0 cat /etc/shadow > /dev/null 2>&1
rc=$?
if [[ $rc -eq 0 ]]
then
  echo 'FAIL: program has priviledge'
fi
