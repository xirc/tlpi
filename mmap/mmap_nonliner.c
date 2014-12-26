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
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>
#include <fcntl.h>


int
main(int argc __attribute__((unused)),
     char *argv[] __attribute__((unused)))
{
    int retc;
    int fd;
    long pagesize;
    char filepath[PATH_MAX];
    void *addr, *addr1, *addr2, *addr3;
    char *buf;
    long i, nfails;

    pagesize = sysconf(_SC_PAGESIZE);
    if (pagesize == -1) {
        perror("sysconf");
        exit(EXIT_FAILURE);
    }

    /* Create temporary file */
    (void) strncpy(filepath, "/tmp/mmapXXXXXX", PATH_MAX);
    filepath[PATH_MAX - 1] = '\0';
    fd = mkstemp(filepath);
    if (fd == -1) {
        perror("mkstemp");
        exit(EXIT_FAILURE);
    }
    /* Unlink temporary file */
    retc = unlink(filepath);
    if (retc == -1) {
        perror("unlink");
        exit(EXIT_FAILURE);
    }

    /* Make contents of file */
    buf = malloc(pagesize);
    if (buf == NULL) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }
#define write_file(val) \
    do { \
        (void) memset(buf, val, pagesize); \
        if (write(fd, buf, pagesize) == -1) { \
            perror("write"); \
            exit(EXIT_FAILURE); \
        }\
    } while (0)
    write_file(0x11);
    write_file(0x22);
    write_file(0x33);
#undef write_file
    free(buf);

    /*
     * mmap file (3 pages)
     * memory page #1  <->  file page #3
     * memory page #2  <->  file page #1
     * memory page #3  <->  file page #2
     */
    addr = mmap(NULL, 3 * pagesize,
            PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (addr == MAP_FAILED) {
        perror("mmap");
        exit(EXIT_FAILURE);
    }

#define mmap_file(idx, mp, fp) \
    do { \
        addr##idx = mmap(addr + mp * pagesize, pagesize, \
                PROT_READ | PROT_WRITE, MAP_SHARED | MAP_FIXED, \
                fd, fp * pagesize); \
        if (addr##idx == MAP_FAILED) { \
            perror("map - ###idx"); \
            exit(EXIT_FAILURE); \
        } \
    } while (0)

    mmap_file(1, 0, 2);
    mmap_file(2, 1, 0);
    mmap_file(3, 2, 1);

#undef mmap_file

    /* Close the file */
    retc = close(fd);
    if (retc == -1) {
        perror("close");
        exit(EXIT_FAILURE);
    }

    /* Test contents */

#define mmap_test(idx, p, val) \
    do { \
        if (*(p) != (val)) { \
            fprintf(stderr, \
                "#" #idx " Unexpected value %08X, expects %08X\n", \
                *(p), (val)); \
            ++nfails; \
        } \
    } while (0)

    nfails = 0;
    for (i = 0; i < 3 * pagesize; ++i) {
        if (i < pagesize) {
            mmap_test(1, (char*)addr + i, 0x33);
        } else if (i < 2 * pagesize) {
            mmap_test(2, (char*)addr + i, 0x11);
        } else {
            mmap_test(3, (char*)addr + i, 0x22);
        }
    }
    for (i = 0; i < pagesize; ++i) {
        mmap_test(4, (char*)addr1 + i, 0x33);
        mmap_test(5, (char*)addr2 + i, 0x11);
        mmap_test(6, (char*)addr3 + i, 0x22);
    }

#undef mmap_test

    printf("%s\n", (nfails == 0) ? "PASS" : "FAIL");

    exit(EXIT_SUCCESS);
}
