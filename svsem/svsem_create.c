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


/* svsem_create.c

   Experiment with the use of semget() to create a System V semaphore set.

   Usage as shown in usageError().
*/


#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>


static void
usage(FILE *fp, const char *progname)
{
    fprintf(fp, "Usage: %s [-cx] {-f pathname | -k key | -p} "
                            "num-sems [octal-perms]\n", progname);
    fprintf(fp, "    -c           Use IPC_CREAT flag\n");
    fprintf(fp, "    -x           Use IPC_EXCL flag\n");
    fprintf(fp, "    -f pathname  Generate key using ftok()\n");
    fprintf(fp, "    -k key       Use 'key' as key\n");
    fprintf(fp, "    -p           Use IPC_PRIVATE key\n");
}


static int
read_uint(char const *str, int const base)
{
    int val;
    char *endptr;

    if (str == NULL || str[0] == '\0') {
        return -1;
    }

    errno = 0;
    val = strtol(str, &endptr, base);
    if (errno != 0) {
        return -1;
    }

    if (*endptr != '\0') {
        return -1;
    }

    return val;
}


int
main(int argc, char *argv[])
{
    int num_key_flags;            /* Counts -f, -k, and -p options */
    int flags, semid, num_sems, opt;
    unsigned int perms;
    long lkey;
    key_t key;

    /* Parse command-line options and arguments */
    num_key_flags = 0;
    flags = 0;
    while ((opt = getopt(argc, argv, "cf:k:px")) != -1) {
        switch (opt) {
        case 'c':
            flags |= IPC_CREAT;
            break;
        case 'f':               /* -f pathname */
            key = ftok(optarg, 1);
            if (key == -1) {
                perror("ftok");
                exit(EXIT_FAILURE);
            }
            num_key_flags++;
            break;
        case 'k':               /* -k key (octal, decimal or hexadecimal) */
            if (sscanf(optarg, "%li", &lkey) != 1) {
                fprintf(stderr, "-k option requires a numeric argument\n");
                usage(stderr, argv[0]);
                exit(EXIT_FAILURE);
            }
            key = lkey;
            num_key_flags++;
            break;
        case 'p':
            key = IPC_PRIVATE;
            num_key_flags++;
            break;
        case 'x':
            flags |= IPC_EXCL;
            break;
        default:
            usage(stderr, argv[0]);
            exit(EXIT_FAILURE);
        }
    }

    if (num_key_flags != 1) {
        fprintf(stderr, "Exactly one of the options -f, -k, "
                        "or -p must be supplied\n");
        usage(stderr, argv[0]);
        exit(EXIT_FAILURE);
    }

    if (optind >= argc) {
        fprintf(stderr, "Must specify number of semaphores\n");
        usage(stderr, argv[0]);
        exit(EXIT_FAILURE);
    }

    num_sems = atoi(argv[optind]);
    perms = (argc <= optind + 1) ? (S_IRUSR | S_IWUSR) :
                read_uint(argv[optind + 1], 8);

    semid = semget(key, num_sems, flags | perms);
    if (semid == -1) {
        perror("semget");
        exit(EXIT_FAILURE);
    }

    printf("%d\n", semid);      /* On success, display semaphore set id */
    exit(EXIT_SUCCESS);
}
