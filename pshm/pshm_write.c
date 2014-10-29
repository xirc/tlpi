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


/* pshm_write.c

   Usage: pshm_write shm-name string

   Copy 'string' into the POSIX shared memory object named in 'shm-name'.

   See also pshm_read.c.
*/


#include <sys/types.h>
#include <sys/mman.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>


static void
usage(FILE *fp, const char *progname)
{
    fprintf(fp, "Usage: %s shm-name string\n", progname);
}


int
main(int argc, char *argv[])
{
    int fd;
    size_t len;     /* Size of shared memory object */
    char *addr;

    if (argc == 2 && strcmp(argv[1], "--help") == 0) {
        usage(stderr, argv[0]);
        exit(EXIT_FAILURE);
    }

    if (argc != 3) {
        usage(stderr, argv[0]);
        exit(EXIT_FAILURE);
    }

    fd = shm_open(argv[1], O_RDWR, 0);    /* Open existing object */
    if (fd == -1) {
        perror("shm_open");
        exit(EXIT_FAILURE);
    }

    len = strlen(argv[2]);
    if (ftruncate(fd, len) == -1) {     /* Resize object to hold string */
        perror("ftruncate");
        exit(EXIT_FAILURE);
    }
    printf("Resized to %ld bytes\n", (long) len);

    addr = mmap(NULL, len, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (addr == MAP_FAILED) {
        perror("mmap");
        exit(EXIT_FAILURE);
    }

    if (close(fd) == -1) {      /* 'fd' is no longer needed */
        perror("close");
        exit(EXIT_FAILURE);
    }

    printf("copying %ld bytes\n", (long) len);
    memcpy(addr, argv[2], len);     /* Copy string to shared memory */

    exit(EXIT_SUCCESS);
}
