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


/* t_mmap.c

   Demonstrate the use of mmap() to create a shared file mapping.
*/


#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define MEM_SIZE 10


int
main(int argc, char *argv[])
{
    char *addr;
    int fd;

    if (argc < 2 || strcmp(argv[1], "--help") == 0) {
        fprintf(stderr, "Usage: %s file [new-value]\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    fd = open(argv[1], O_RDWR);
    if (fd == -1) {
        perror("open");
        exit(EXIT_FAILURE);
    }

    addr = mmap(NULL, MEM_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (addr == MAP_FAILED) {
        perror("mmap");
        exit(EXIT_FAILURE);
    }

    if (close(fd) == -1) {  /* No longer need 'fd' */
        perror("close");
        exit(EXIT_FAILURE);
    }

    printf("Current string %.*s\n", MEM_SIZE, addr);
            /* Secure practice: output at most MEM_SIZE bytes */

    if (argc > 2) {     /* Update contents of region */
        if (strlen(argv[2]) >= MEM_SIZE) {
            fprintf(stderr, "'new-value' too long\n");
            exit(EXIT_FAILURE);
        }

        memset(addr, 0, MEM_SIZE);      /* Zero out region */
        strncpy(addr, argv[2], MEM_SIZE - 1);
        if (msync(addr, MEM_SIZE, MS_SYNC) == -1) {
            perror("msync");
            exit(EXIT_FAILURE);
        }

        printf("Copied \"%s\" to shared memory\n", argv[2]);
    }

    if (munmap(addr, MEM_SIZE) == -1) {
        perror("munmap");
        exit(EXIT_FAILURE);
    }

    exit(EXIT_SUCCESS);
}
