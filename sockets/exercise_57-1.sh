#!/usr/bin/env bash

set -x

# BLOCK does not happen.
./ud_dgram 100 10 1

# BLOCK happens.
./ud_dgram 100000 4000 1

exit 0
