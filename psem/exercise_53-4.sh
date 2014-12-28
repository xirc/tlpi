#!/usr/bin/env bash

COUNT=2500000
time ./svsem_spec ${COUNT}
time ./psem_spec /psem_spec ${COUNT}
