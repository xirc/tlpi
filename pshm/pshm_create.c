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


/* pshm_create.c

   Create a POSIX shared memory object with specified size and permissions.

   Usage as shown in usageError().
*/


#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <sys/mman.h>


static void
usage(FILE *fp, const char *progname)
{
    fprintf(fp, "Usage: %s [-cx] name size [octal-perms]\n", progname);
    fprintf(fp, "    -c   Create shared memory (O_CREAT)\n");
    fprintf(fp, "    -x   Create exclusively (O_EXCL)\n");
}


int
main(int argc, char *argv[])
{
    int flags, opt, fd;
    mode_t perms;
    size_t size;
    void *addr;

    flags = O_RDWR;
    while ((opt = getopt(argc, argv, "cx")) != -1) {
        switch (opt) {
        case 'c':   flags |= O_CREAT;        break;
        case 'x':   flags |= O_EXCL;         break;
        default:    usage(stderr, argv[0]);  exit(EXIT_FAILURE);
        }
    }

    if (optind + 1 >= argc) {
        usage(stderr, argv[0]);
        exit(EXIT_FAILURE);
    }

    size = strtol(argv[optind + 1], NULL, 0);
    perms = (argc <= optind + 2) ? (S_IRUSR | S_IWUSR) :
        strtoul(argv[optind + 2], NULL, 8);
    if (perms > 0777) {
        fprintf(stderr, "Invalid number octal-perms: %s\n",
                argv[optind + 2]);
        exit(EXIT_FAILURE);
    }

    /* Create shared memory object and set its size */
    fd = shm_open(argv[optind], flags, perms);
    if (fd == -1) {
        perror("shm_open");
        exit(EXIT_FAILURE);
    }

    if (ftruncate(fd, size) == -1) {
        perror("ftruncate");
        exit(EXIT_FAILURE);
    }

    /* Map shared memory object */
    addr = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (addr == MAP_FAILED) {
        perror("mmap");
        exit(EXIT_FAILURE);
    }

    exit(EXIT_SUCCESS);
}
