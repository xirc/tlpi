#!/usr/bin/env bash

TMPFILE=$(mktemp)
trap "rm -f ${TMPFILE}" EXIT

# Send whole file
rm -f ${TMPFILE}
./m_sendfile m_sendfile ${TMPFILE} 1000000
if ! cmp m_sendfile ${TMPFILE} ; then
    echo 'FAIL #1'
    exit 1
fi

# Send first 100 bytes
rm -f ${TMPFILE}
./m_sendfile m_sendfile ${TMPFILE} 100
if ! cmp -s <(head --bytes 100 m_sendfile) ${TMPFILE} ; then
    echo 'FAIL #2'
    exit 1
fi

# Send 8100-8300 bytes
rm -f ${TMPFILE}
./m_sendfile m_sendfile ${TMPFILE} 200 8100
if ! cmp -s <(head --bytes 8300 m_sendfile |  tail --bytes 200) ${TMPFILE} ; then
    echo 'FAIL #3'
    exit 1
fi

echo 'PASS'
exit 0
