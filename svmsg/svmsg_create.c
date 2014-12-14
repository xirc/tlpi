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


/* svmsg_create.c

   Experiment with the use of msgget() to create a System V message queue.
*/


#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <getopt.h>


/* Print usage info */
static void
usage(FILE* fp, char const * progname)
{
    fprintf(fp, "Usage: %s [-cx] {-f pathname | -k key | -p} "
                "[octal-perms]\n", progname);
    fprintf(fp, "    -c           Use IPC_CREAT flag\n");
    fprintf(fp, "    -x           Use IPC_EXCL flag\n");
    fprintf(fp, "    -f pathname  Generate key using ftok()\n");
    fprintf(fp, "    -k key       Use 'key' as key\n");
    fprintf(fp, "    -p           Use IPC_PRIVATE key\n");
}


static int
get_octal_uint(char const *str)
{
    int val;
    char *endptr;

    if (str == NULL || str[0] == '\0') {
        /* Empty string */
        return -1;
    }

    errno = 0;
    val = strtol(str, &endptr, 8);
    if (errno != 0) {
        /* Some error occured */
        return -1;
    }

    if (str == endptr) {
        /* No digits were found */
        return -1;
    }

    if (*endptr != '\0') {
        /* Further characters after number */
        return -1;
    }

    if (val < 0) {
        /* Negative number are given */
        return -1;
    }

    return val;
}


int
main(int argc, char *argv[])
{
    int num_key_flags;      /* Counts -f, -k, and -p options */
    int flags, msqid, opt;
    int input_perms;
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

        case 'f':       /* -f pathname */
            key = ftok(optarg, 1);
            if (key == -1) {
                perror("ftok");
                exit(EXIT_FAILURE);
            }
            ++num_key_flags;
            break;

        case 'k':       /* -k key (octal, decimal or hexadecimal) */
            if (sscanf(optarg, "%li", &lkey) != 1) {
                fprintf(stderr, "-k option requires a numeric argument\n");
                usage(stderr, argv[0]);
                exit(EXIT_FAILURE);
            }
            key = lkey;
            ++num_key_flags;
            break;

        case 'p':
            key = IPC_PRIVATE;
            ++num_key_flags;
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

    if (optind == argc) {
        perms = S_IRUSR | S_IWUSR;
    } else {
        input_perms = get_octal_uint(argv[optind]);
        if (input_perms < 0 || input_perms > 0777) {
            fprintf(stderr, "0 <= octal-perms %s <= 0777\n", argv[optind]);
            exit(EXIT_FAILURE);
        }
        perms = (unsigned int) input_perms;
    }

    msqid = msgget(key, flags | perms);
    if (msqid == -1) {
        perror("msgget");
        exit(EXIT_FAILURE);
    }

    printf("%d\n", msqid);

    exit(EXIT_SUCCESS);
}
