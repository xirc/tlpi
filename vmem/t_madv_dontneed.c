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
#include <sys/mman.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>


int
main(int argc __attribute__((unused)),
     char *argv[] __attribute__((unused)))
{
#define SIZE 16
    int i;
    char *addr;

    addr = mmap(NULL, SIZE, PROT_READ | PROT_WRITE,
            MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (addr == MAP_FAILED) {
        perror("mmap");
        exit(EXIT_FAILURE);
    }

    for (i = 0 ; i < SIZE; ++i) {
        addr[i] = i;
    }

    printf("BEFORE:\n");
    for (i = 0; i < SIZE; ++i) {
        printf("%p:    %08X\n", addr + i, addr[i]);
    }

    printf("madvise MADV_DONTNEED\n");
    if (madvise(addr, SIZE, MADV_DONTNEED) == -1) {
        perror("madvise");
        exit(EXIT_FAILURE);
    }

    printf("AFTER:\n");
    for (i = 0; i < SIZE; ++i) {
        printf("%p:    %08X\n", addr + i, addr[i]);
    }

    if (munmap(addr, SIZE) == -1) {
        perror("munmap");
        exit(EXIT_FAILURE);
    }

    exit(EXIT_SUCCESS);
}
