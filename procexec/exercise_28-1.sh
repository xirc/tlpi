#!/usr/bin/env bash

cat <<EOT
fork  100000 times (process size ~= 100M)
EOT
time ./spec_fork -n 100000 -s 100

cat <<EOT

---
vfork 100000 times (process size ~= 100M)
EOT
time ./spec_fork -n 100000 -s 100 -v
