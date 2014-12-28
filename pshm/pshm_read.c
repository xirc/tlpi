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


/* pshm_read.c

   Usage: pshm_read shm-name

   Copy the contents of the POSIX shared memory object named in
   'name' to stdout.

   See also pshm_write.c.
*/


#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>


int
main(int argc, char *argv[])
{
    int fd;
    char *addr;
    struct stat sb;

    if (argc != 2 || strcmp(argv[1], "--help") == 0) {
        fprintf(stderr, "Usage: %s shm-name\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    fd = shm_open(argv[1], O_RDONLY, 0);
    if (fd == -1) {
        perror("shm_open");
        exit(EXIT_FAILURE);
    }


    /* Use shared memory object size as length argument for mmap() and
     * as number of bytes to write() */
    if (fstat(fd, &sb) == -1) {
        perror("fstat");
        exit(EXIT_FAILURE);
    }

    addr = mmap(NULL, sb.st_size, PROT_READ, MAP_SHARED, fd, 0);
    if (addr == MAP_FAILED) {
        perror("mmap");
        exit(EXIT_FAILURE);
    }

    if (close(fd) == -1) {
        perror("close");
        exit(EXIT_FAILURE);
    }

    write(STDOUT_FILENO, addr, sb.st_size);
    printf("\n");

    exit(EXIT_SUCCESS);
}
