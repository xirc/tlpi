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
#include <limits.h>
#include <string.h>
#include <signal.h>


static void
test_access(void const *head, off_t const offset)
{
    char *p;
    char val;
    p = (char*) head + offset;
    val = *p;
    printf("%p (off=%8ld): %08X\n",
            p, (long)offset, val);
}


int
main(int argc __attribute__((unused)),
     char *argv[] __attribute__((unused)))
{
#define BUFSIZE 1024
    int fd;
    long pagesize;
    char filepath[PATH_MAX];
    void *addr;
    int retc;
    char buf[BUFSIZE];
    off_t offset;
    int code;

    if (argc < 2 || strcmp(argv[1], "--help") == 0) {
        fprintf(stderr, "Usage: %s {0:sigbus,1:sigsegv}\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    code = atoi(argv[1]);

    pagesize = sysconf(_SC_PAGESIZE);
    if (pagesize == -1) {
        perror("sysconf");
        exit(EXIT_FAILURE);
    }
    printf("Page Size: %ld\n", pagesize);

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
    /* Write 0xFF to file */
    (void) memset(buf, 0xFF, BUFSIZE);
    for (offset = 0; offset < pagesize / 2; offset += BUFSIZE) {
        if (write(fd, buf, BUFSIZE) == -1) {
            perror("write");
            exit(EXIT_FAILURE);
        }
    }
    /* Set a half of pagesize as file size */
    retc = ftruncate(fd, pagesize / 2);
    if (retc == -1) {
        perror("ftruncate");
        exit(EXIT_FAILURE);
    }

    /* mmap file */
    addr = mmap(NULL, 2 * pagesize,
            PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (addr == MAP_FAILED) {
        perror("mmap");
        exit(EXIT_FAILURE);
    }

    /* Close the file */
    retc = close(fd);
    if (retc == -1) {
        perror("close");
        exit(EXIT_FAILURE);
    }

    /* Access test */
    test_access(addr, 0);
    test_access(addr, pagesize / 2 - 1);
    test_access(addr, pagesize / 2);
    test_access(addr, pagesize - 1);
    switch (code) {
    case 0:     /* SIGBUS */
        test_access(addr, pagesize);
        break;
    case 1:     /* SIGSEGV */
        test_access(addr, pagesize * 2);
        break;
    default:
        break;
    }

    exit(EXIT_SUCCESS);
}
