#!/usr/bin/env sh

tearup () {
mkdir dir1
cat >dir1/xyz <<EOT
#!/bin/sh
echo 'dir1/xyz'
EOT
chmod 644 dir1/xyz

mkdir dir2
cat >dir2/xyz <<EOT
#!/bin/sh
echo 'dir2/xyz'
EOT
chmod 755 dir2/xyz
}

teardown () {
rm dir1/xyz
rm dir2/xyz
rmdir dir1
rmdir dir2
}

trap teardown EXIT
tearup


# main
BASE_PATH=$PATH
echo 'PATH=...:./dir1:./dir2'
PATH=$BASE_PATH:./dir1:./dir2
./t_execlp xyz
echo 'PATH=...:./dir1'
PATH=$BASE_PATH:./dir1
./t_execlp xyz
echo 'PATH=...:./dir2'
PATH=$BASE_PATH:./dir2
./t_execlp xyz
