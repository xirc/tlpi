#!/usr/bin/env bash

orig_date=$(date -uR)

(
  cd /tmp

  rc=0
  date -s '2018-02-01 21:39' > /dev/null 2>&1
  rc=$?
  if [[ $rc -ne 1 ]]; then
    echo "unexpected: rc=$rc" >> /dev/stderr
  fi
  
  sudo date -s '2018-02-01 21:39' > /dev/null 2>&1
  rc=$?
  if [[ $rc -ne 0 ]]; then
    echo "unexpected: rc=$rc" >> /dev/stderr
  fi

  dt=$(date -R)
  if [[ "$dt" != 'Thu, 01 Feb 2018 21:39:00 +0000' ]]; then
    echo "unexpected: date=$dt" >> /dev/stderr
  fi

  f=$(which date)
  cp $f .
  sudo setcap "cap_sys_time=pe" date
  cap=$(getcap date)
  if [[ "$cap" != 'date = cap_sys_time+ep' ]]; then
    echo "unexpected: cap=$cap" >> /dev/stderr
  fi

  ./date -s '2010-12-29 15:55' > /dev/null 2>&1
  rc=$?
  if [[ $rc -ne 0 ]]; then
    echo "unexpected: rc=$rc" >> /dev/stderr
  fi
)

sudo date -s "$orig_date"
