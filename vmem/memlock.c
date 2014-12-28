/*************************************************************************\
*                  Copyright (C) Michael Kerrisk, 2014.                   *
*                                                                         *
* This program is free software. You may use, modify, and redistribute it *
* under the terms of the GNU Affero General Public License as published   *
* by the Free Software Foundation, either version 3 or (at your option)   *
* any later version. This program is distributed without any warranty.    *
* See the file COPYING.agpl-v3 for details.                               *
\*************************************************************************/
/*************************************************************************\
*                  Copyright (C) xirc, 2014.                              *
* See above.                                                              *
\*************************************************************************/


/* memlock.c

   Demonstrate the use of mlock(), to place memory locks, and mincore(), to
   retrieve memory-residence information about the calling processes virtual
   memory pages.

   Note: some UNIX implementations do not provide mincore().

   The madvise() system call is upported on Linux since kernel 2.4.
*/


#define _BSD_SOURCE     /* Get mincore() declaration and MAP_ANONYMOUS
                           definition from <sys/mman.h> */

#include <sys/types.h>
#include <sys/mman.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>


static unsigned long
toulong(char const *str, char const *name)
{
    char *endptr;
    long val;

    /* Empty string */
    if (str == NULL || str[0] == '\0') {
        goto FAIL;
    }

    errno = 0;
    val = strtoul(str, &endptr, 0);
    if (errno != 0) {
        /* A error occurs */
        goto FAIL;
    }

    /* Further characters after number */
    if (*endptr != '\0') {
        goto FAIL;
    }

    return val;

FAIL:
    fprintf(stderr, "Invalid number '%s' %s\n", name, str);
    exit(EXIT_FAILURE);
}


/* Display residency of pages in range [addr ... (addr + length - 1)] */
static void
display_mincore(char *addr, size_t length)
{
    unsigned char *vec;
    long page_size, num_pages, j;

    page_size = sysconf(_SC_PAGESIZE);
    if (page_size == -1) {
        perror("sysconf _SC_PAGESIZE");
        exit(EXIT_FAILURE);
    }

    num_pages = (length + page_size - 1) / page_size;
    vec = malloc(num_pages);
    if (vec == NULL) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }

    if (mincore(addr, length, vec) == -1) {
        perror("mincore");
        exit(EXIT_FAILURE);
    }

    for (j = 0; j < num_pages; ++j) {
        if (j % 64 == 0) {
            printf("%s%10p: ", (j == 0) ? "" : "\n", addr + (j * page_size));
        }
        printf("%c", (vec[j] & 1) ? '*' : '.');
    }
    printf("\n");

    free(vec);
}


int
main(int argc, char *argv[])
{
    char *addr;
    size_t len, locklen;
    long page_size, step_size, j;

    if (argc != 4 || strcmp(argv[1], "--help") == 0) {
        fprintf(stderr, "Usage: %s num-pages lock-page-step lock-page-len\n",
                argv[0]);
        exit(EXIT_FAILURE);
    }

    page_size = sysconf(_SC_PAGESIZE);
    if (page_size == -1) {
        perror("sysconf _SC_PAGESIZE");
        exit(EXIT_FAILURE);
    }

    len       = toulong(argv[1], "num-pages") * page_size;
    step_size = toulong(argv[2], "lock-page-step") * page_size;
    locklen   = toulong(argv[3], "lock-page-len") * page_size;

    addr = mmap(NULL, len, PROT_READ, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    if (addr == MAP_FAILED) {
        perror("mmap");
        exit(EXIT_FAILURE);
    }

    printf("Allocated %ld (%#lx) bytes string at %p\n",
            (long)len, (unsigned long) len, addr);

    printf("Becore mlock:\n");
    display_mincore(addr, len);

    /* Lock pages specified by command line arguments into memory */
    for (j = 0; j + locklen <= len; j += step_size) {
        if (mlock(addr + j, locklen) == -1) {
            perror("mlock");
            exit(EXIT_FAILURE);
        }
    }

    printf("After mlock:\n");
    display_mincore(addr, len);

    exit(EXIT_SUCCESS);
}
