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


/* svshm_create.c

   Experiment with the use of shmget() to create a System V shared memory
   segment.

   Usage as shown in usageError().
*/


#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>


static void
usage(FILE *fp, const char *prog_name)
{
    fprintf(fp, "Usage: %s [-cx] {-f pathname | -k key | -p} "
                            "seg-size [octal-perms]\n", prog_name);
    fprintf(fp, "    -c           Use IPC_CREAT flag\n");
    fprintf(fp, "    -x           Use IPC_EXCL flag\n");
    fprintf(fp, "    -f pathname  Generate key using ftok()\n");
    fprintf(fp, "    -k key       Use 'key' as key\n");
    fprintf(fp, "    -p           Use IPC_PRIVATE key\n");
}


static int
safe_strtou(char const *str, int const base, unsigned int *val)
{
    long lv;
    char *endptr;

    /* NULL or Empty string */
    if (str == NULL || *str == '\0') {
        return -1;
    }

    errno = 0;
    lv = strtol(str, &endptr, base);
    if (errno != 0) {
        return -1;
    }

    /* Further caracters after number */
    if (*endptr != '\0') {
        return -1;
    }

    /* OVERFLOW ? */
    if (lv < 0 || lv > LONG_MAX) {
        return -1;
    }

    *val = (int) lv;
    return 0;
}


int
main(int argc, char *argv[])
{
    int num_key_flags;            /* Counts -f, -k, and -p options */
    int flags, shmid;
    unsigned int seg_size;
    unsigned int perms;
    long lkey;
    key_t key;
    int opt;                    /* Option character from getopt() */

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
                perror("fork");
                exit(EXIT_FAILURE);
            }
            num_key_flags++;
            break;

        case 'k':               /* -k key (octal, decimal or hexadecimal) */
            if (sscanf(optarg, "%li", &lkey) != 1) {
                fprintf(stderr,
                        "-k option requires a numeric argument\n");
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
        fprintf(stderr, "Size of segment must be specified\n");
        exit(EXIT_FAILURE);
    }

    // Parse seg-size
    if (safe_strtou(argv[optind], 10, &seg_size) == -1 ||
        seg_size == 0)
    {
        fprintf(stderr, "invalid seg-size %s\n", argv[optind]);
        exit(EXIT_FAILURE);
    }

    if (argc <= optind + 1) {
        perms = (S_IRUSR | S_IWUSR);
    } else {
        if (safe_strtou(argv[optind + 1], 8, &perms) == -1) {
            fprintf(stderr, "invalid octal-perms %s\n", argv[optind + 1]);
            exit(EXIT_FAILURE);
        }
    }

    shmid = shmget(key, seg_size, flags | perms);
    if (shmid == -1) {
        perror("shmget");
        exit(EXIT_FAILURE);
    }

    printf("%d\n", shmid);      /* On success, display shared mem. id */
    exit(EXIT_SUCCESS);
}
