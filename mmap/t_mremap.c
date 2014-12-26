/*************************************************************************\
*                  Copyright (C) xirc, 2014.                              *
*                                                                         *
* This program is free software. You may use, modify, and redistribute it *
* under the terms of the GNU Affero General Public License as published   *
* by the Free Software Foundation, either version 3 or (at your option)   *
* any later version. This program is distributed without any warranty.    *
* See the file COPYING.agpl-v3 for details.                               *
\*************************************************************************/


#define _GNU_SOURCE
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/mman.h>
#include <fcntl.h>


int
main(int argc __attribute__((unused)),
     char *argv[] __attribute__((unused)))
{
    int fd;
    void *addr, *mapped, *mapped2;
    long pagesize;

    pagesize = sysconf(_SC_PAGESIZE);
    if (pagesize == -1) {
        perror("sysconf");
        exit(EXIT_FAILURE);
    }
    printf("Page Size: %ld\n", pagesize);

    fd = open("/dev/zero", O_RDWR);
    if (fd == -1) {
        perror("open");
        exit(EXIT_FAILURE);
    }

    /* Memory mapping */
    addr = &addr;
    mapped = mmap(addr, pagesize, PROT_READ | PROT_WRITE, MAP_PRIVATE, fd, 0);
    if (mapped == MAP_FAILED) {
        perror("mmap");
        exit(EXIT_FAILURE);
    }
    printf("mmap(addr=%p, length=%ld, prot=PROT_READ,\n"
           "     flags=MAP_PRIVATE, fd=(fd of /dev/zero), offset=0)\n", addr, pagesize);
    printf(">> %p\n", mapped);

    /* Memory ReMapping */
    mapped2 = mremap(mapped, pagesize, pagesize * 2, MREMAP_MAYMOVE);
    if (mapped2 == MAP_FAILED) {
        perror("mremap");
        exit(EXIT_FAILURE);
    }
    printf("mremap(old_addr=%p, old_size=%ld, new_size=%ld, flags=MREMAP_MAYMOVE\n",
            mapped, pagesize, pagesize * 2);
    printf(">> %p\n", mapped2);

    if (munmap(mapped2, pagesize) == -1) {
        perror("munmap");
        exit(EXIT_FAILURE);
    }

    exit(EXIT_SUCCESS);
}
