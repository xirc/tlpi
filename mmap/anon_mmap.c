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


/* anon_mmap.c

   Demonstrate how to share a region of mapped memory between a parent and
   child process without having to create a mapped file, either through the
   creation of an anonymous memory mapping or through the mapping of /dev/zero.
*/


#ifdef USE_MAP_ANON
#define _BSD_SOURCE     /* Get AMP_ANONYMOUS definition */
#endif

#include <sys/types.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>


int
main(int argc __attribute__((unused)),
     char *argv[] __attribute__((unused)))
{
    int *addr;          /* Pointer to shared memory region */

#ifdef USE_MAP_ANON
    /* Use MAP_ANONYMOUS */
    addr = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE,
                MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    if (addr == MAP_FAILED) {
        perror("mmap");
        exit(EXIT_FAILURE);
    }
#else
    /* Use mapping /dev/zero */
    int fd;

    fd = open("/dev/zero", O_RDWR);
    if (fd == -1) {
        perror("open");
        exit(EXIT_FAILURE);
    }
    addr = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE,
                MAP_SHARED, fd, 0);
    if (addr == MAP_FAILED) {
        perror("mmap");
        exit(EXIT_FAILURE);
    }

    if (close(fd) == -1) {      /* No longer needed */
        perror("close");
        exit(EXIT_FAILURE);
    }
#endif

    *addr = 1;      /* Initialize integer in mapped region */

    switch (fork()) {       /* Parent and child share mapping */
    case -1:
        perror("fork");
        exit(EXIT_FAILURE);
    case 0:     /* Child: increment shared integer and exit */
        printf("Child started, value = %d\n", *addr);
        ++(*addr);
        if (munmap(addr, sizeof(int)) == -1) {
            perror("munmap");
            exit(EXIT_FAILURE);
        }
        exit(EXIT_SUCCESS);
    default:    /* Parent: wait for child to terminate */
        if (wait(NULL) == -1) {
            perror("wait");
            exit(EXIT_FAILURE);
        }
        printf("In parent, value = %d\n", *addr);
        if (munmap(addr, sizeof(int)) == -1) {
            perror("munmap");
            exit(EXIT_FAILURE);
        }
        break;
    }

    /* Parent comes here */
    exit(EXIT_SUCCESS);
}
