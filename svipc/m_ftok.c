/*************************************************************************\
*                  Copyright (C) xirc, 2014.                              *
*                                                                         *
* This program is free software. You may use, modify, and redistribute it *
* under the terms of the GNU Affero General Public License as published   *
* by the Free Software Foundation, either version 3 or (at your option)   *
* any later version. This program is distributed without any warranty.    *
* See the file COPYING.agpl-v3 for details.                               *
\*************************************************************************/


#define _BSD_SOURCE
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ipc.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <limits.h>


static key_t m_ftok(char const *filepath, int proj);


static char filepath[PATH_MAX];


static void
cleanup(void)
{
    (void) unlink(filepath);
}


int
main(int argc __attribute__((unused)),
     char *argv[] __attribute__((unused)))
{
    key_t key, mkey;
    int proj;
    int fd;
    struct stat s;

    /* Set proj randomly */
    fd = open("/dev/urandom", O_RDONLY);
    if (fd == -1) {
        perror("open /dev/urandom");
        exit(EXIT_FAILURE);
    }
    if (read(fd, &proj, sizeof(proj)) == -1) {
        perror("read /dev/urandom");
        exit(EXIT_FAILURE);
    }
    if (close(fd) == -1) {
        perror("close /dev/urandom");
        exit(EXIT_FAILURE);
    }

    /* Make filepath corresponding to PID */
    snprintf(filepath, PATH_MAX, "%s.%ld", __FILE__, (long) getpid());
    fd = open(filepath, O_CREAT | O_EXCL | O_RDWR |
                        S_IRUSR | S_IWUSR | S_IRGRP);   /* rw-r----- */
    if (fd == -1) {
        fprintf(stderr, "open %s: %s\n", filepath, strerror(errno));
        exit(EXIT_FAILURE);
    }
    atexit(cleanup);
    if (close(fd) == -1) {
        fprintf(stderr, "close %s: %s\n", filepath, strerror(errno));
        exit(EXIT_FAILURE);
    }

    /* Get stat of 'filepath' */
    if (stat(filepath, &s) == -1) {
        perror("stat");
        exit(EXIT_FAILURE);
    }

    /* ftok generate key based on
     *   proj:
     *      lower 8bit
     *   file:
     *      minor device number (8bit)
     *      lower 16 bit of i-node number
     */
    key = ftok(filepath, proj);
    if (key == -1) {
        perror("ftok");
        exit(EXIT_FAILURE);
    }
    mkey = m_ftok(filepath, proj);
    if (key == -1) {
        perror("m_ftok");
        exit(EXIT_FAILURE);
    }

    printf("ftok:   0x%x\n", key);
    printf("m_ftok: 0x%x\n", mkey);
    printf("%s\n", (key == mkey) ? "OK" : "NG");

    exit(EXIT_SUCCESS);
}


static key_t
m_ftok(char const *filepath, int proj)
{
    key_t key;
    struct stat st;

    if (stat(filepath, &st) == -1) {
        return -1;
    }

    key = (proj & 0xFF) << 24 |
           (minor(st.st_dev) & 0xFF) << 16 |
           (st.st_ino & 0xFFFF);
    return key;
}
