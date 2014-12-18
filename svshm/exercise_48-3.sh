#!/bin/bash

cc_xfr () {
  local of=$1
  local if=$2
  local flg=$3
  local CFLAGS="-std=c99 -D_XOPEN_SOURCE=700 -g -Wall -Wextra"
  gcc ${CFLAGS} ${flg} -o ${of} ${if}
}

make_xfr () {
  local bufsize=$1
  cc_xfr binary_sems.o "-c binary_sems.c"
  cc_xfr svshm_xfr_writer "svshm_xfr_writer.c binary_sems.o" "-DBUF_SIZE=${bufsize}"
  cc_xfr svshm_xfr_reader "svshm_xfr_reader.c binary_sems.o" "-DBUF_SIZE=${bufsize}"
}

clean_xfr () {
  rm -f binary_sems.o
  rm -f svshm_xfr_writer
  rm -f svshm_xfr_reader
}

itmpf=$(mktemp)
otmpf=$(mktemp)
cleanup () {
  rm ${itmpf} ${otmpf}
  ipcs -m | awk '{print $2}' | xargs -I% ipcrm -m % >/dev/null 2>&1
  ipcs -s | awk '{print $2}' | xargs -I% ipcrm -s % >/dev/null 2>&1
  clean_xfr
}
trap "cleanup" EXIT
dd if=/dev/urandom of=${itmpf} count=1K bs=1K >/dev/null 2>&1

bsl="1 8 16 32 64 256 1024 8096"
for bs in ${bsl}
do
  echo ===
  echo BUF_SIZE=${bs}
  make_xfr ${bs}
  ./svshm_xfr_writer <${itmpf} &
  # Wait the writer create a IPC shmem and a shsem.
  sleep 1 && time ./svshm_xfr_reader >${otmpf}
  diff -q ${itmpf} ${otmpf}
  truncate --size 0 ${otmpf}
  clean_xfr
  echo
done 2>&1

exit 0
