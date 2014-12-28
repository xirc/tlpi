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


/* psem_create.c

   Create a POSIX named semaphore.

   Usage as shown in usageError().

   On Linux, named semaphores are supported with kernel 2.6 or later, and
   a glibc that provides the NPTL threading implementation.
*/


#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <semaphore.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <limits.h>


static void
usage(FILE *fp, const char *progname)
{
    fprintf(fp, "Usage: %s [-cx] name [octal-perms [value]]\n", progname);
    fprintf(fp, "    -c   Create semaphore (O_CREAT)\n");
    fprintf(fp, "    -x   Create exclusively (O_EXCL)\n");
}


static int
toint(const char *str, const int base, const char *name)
{
    char *endptr;
    long val;

    if (str == NULL || str[0] == '\0') {
        /* Empty string */
        goto FAIL;
    }

    errno = 0;
    val = strtol(str, &endptr, base);
    if (errno != 0) {
        /* Some unexpected errors */
        goto FAIL;
    }

    if (*endptr != '\0') {
        /* Further characters after number */
        goto FAIL;
    }

    if (val > INT_MAX || val < INT_MIN) {
        goto FAIL;
    }

    return (int) val;

FAIL:
    fprintf(stderr, "Invalid number '%s': %s\n", name, str);
    exit(EXIT_FAILURE);
}


static unsigned int
touint(const char *str, const int base, const char *name)
{
    char *endptr;
    unsigned long val;

    if (str == NULL || str[0] == '\0') {
        /* Empty string */
        goto FAIL;
    }

    errno = 0;
    val = strtoul(str, &endptr, base);
    if (errno != 0) {
        /* Some unexpected errors */
        goto FAIL;
    }

    if (*endptr != '\0') {
        /* Further characters after number */
        goto FAIL;
    }

    if (val > UINT_MAX) {
        goto FAIL;
    }

    return (unsigned int) val;

FAIL:
    fprintf(stderr, "Invalid number '%s': %s\n", name, str);
    exit(EXIT_FAILURE);
}


int
main(int argc, char *argv[])
{
    int flags, opt;
    mode_t perms;
    unsigned int value;
    sem_t *sem;

    flags = 0;
    while ((opt = getopt(argc, argv, "cx")) != -1) {
        switch (opt) {
        case 'c':   flags |= O_CREAT;        break;
        case 'x':   flags |= O_EXCL;         break;
        default:    usage(stderr, argv[0]);  exit(EXIT_FAILURE);
        }
    }

    if (optind >= argc) {
        usage(stderr, argv[0]);
        exit(EXIT_FAILURE);
    }

    /*
     * Default permissions are rw-------;
     * default semaphore initialization value is 0.
     */
    perms = (argc <= optind + 1) ? (S_IRUSR | S_IWUSR) :
        touint(argv[optind + 1], 8, "octal-perms");
    value = (argc <= optind + 2) ? 0 :
        toint(argv[optind + 2], 0, "value");

    sem = sem_open(argv[optind], flags, perms, value);
    if (sem == SEM_FAILED) {
        perror("sem_open");
        exit(EXIT_FAILURE);
    }

    exit(EXIT_SUCCESS);
}
