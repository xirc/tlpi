#!/usr/bin/env bash

(
  echo 'Make /tmp/douser and set priviledge it'
  sudo cp ./douser /tmp/douser
  sudo chown root /tmp/douser
  sudo chgrp root /tmp/douser
  sudo chmod u+s /tmp/douser
  
  set -x
  /tmp/douser -u root -- head -n 3 /var/log/messages || echo "FAIL"
  /tmp/douser -u vagrant -- tail -n 3 /var/log/messages && echo "FAIL"
  set +x
)
