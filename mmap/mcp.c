/*************************************************************************\
*                  Copyright (C) xirc, 2014.                              *
*                                                                         *
* This program is free software. You may use, modify, and redistribute it *
* under the terms of the GNU Affero General Public License as published   *
* by the Free Software Foundation, either version 3 or (at your option)   *
* any later version. This program is distributed without any warranty.    *
* See the file COPYING.agpl-v3 for details.                               *
\*************************************************************************/


#include <sys/types.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>


int
main(int argc, char *argv[])
{
#define RWSIZE 1024 /* 1024 bytes */
    char *spath, *dpath;
    int sfd, dfd;
    void *saddr, *daddr;
    struct stat st;
    off_t filesize;

    int retc;
    off_t offset, rems;
    size_t n;

    if (argc == 2 && strcmp(argv[1], "--help") == 0) {
        printf("%s SOURCE DESTINATION\n", argv[0]);
        exit(EXIT_SUCCESS);
    }
    if (argc != 3) {
        printf("%s: missing file operand\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    spath = argv[1];
    dpath = argv[2];

    sfd = open(spath, O_RDONLY);
    if (sfd == -1) {
        perror("open - SOURCE");
        exit(EXIT_FAILURE);
    }
    dfd = open(dpath, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR | S_IRGRP);
    if (dfd == -1) {
        perror("open - DEST");
        exit(EXIT_FAILURE);
    }

    retc = fstat(sfd, &st);
    if (retc == -1) {
        perror("fstat");
        exit(EXIT_FAILURE);
    }
    filesize = st.st_size;
    retc = ftruncate(dfd, filesize);
    if (retc == -1) {
        perror("ftruncate");
        exit(EXIT_FAILURE);
    }

    saddr = mmap(NULL, filesize, PROT_READ, MAP_PRIVATE, sfd, 0);
    if (saddr == MAP_FAILED) {
        perror("mmap - SOURCE");
        exit(EXIT_FAILURE);
    }
    daddr = mmap(NULL, filesize, PROT_READ | PROT_WRITE, MAP_SHARED, dfd, 0);
    if (daddr == MAP_FAILED) {
        perror("mmap - DEST");
        exit(EXIT_FAILURE);
    }

    for (offset = 0; offset < filesize; offset += RWSIZE) {
        rems = (filesize - offset);
        n = (rems > (off_t) RWSIZE) ? RWSIZE : (size_t) rems;
        (void) memcpy(daddr + offset, saddr + offset, n);
    }

    if (munmap(saddr, filesize) == -1) {
        perror("munmap - SOURCE");
        exit(EXIT_FAILURE);
    }
    if (munmap(daddr, filesize) == -1) {
        perror("munmap - DEST");
        exit(EXIT_FAILURE);
    }

    printf("%s: %.*s -> %.*s    %ld bytes copied\n",
            argv[0], PATH_MAX, spath, PATH_MAX, dpath, filesize);

    exit(EXIT_FAILURE);
}
