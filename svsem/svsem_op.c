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


/* svsem_op.c

   Perform groups of operations on a System V semaphore set.

   Usage as shown in usageError().
*/


#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <ctype.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>


/* Maximum operations that we permit for a single semop() */
#define MAX_SEMOPS 1000


static void
usage(FILE *fp, char const *progname)
{
    fprintf(fp, "Usage: %s semid op[,op...] ...\n\n", progname);
    fprintf(fp, "'op' is either: <sem#>{+|-}<value>[n][u]\n");
    fprintf(fp, "            or: <sem#>=0[n]\n");
    fprintf(fp, "       \"n\" means include IPC_NOWAIT in 'op'\n");
    fprintf(fp, "       \"u\" means include SEM_UNDO in 'op'\n");
    fprintf(fp, "The operations in each argument are "
                "performed in a single semop() call\n\n");
    fprintf(fp, "e.g.: %s 12345 0+1,1-2un\n", progname);
    fprintf(fp, "      %s 12345 0=0n 1+1,2-1u 1=0\n", progname);
}


/* Parse comma-delimited operations in 'arg', returning them in the
 * array 'sops'. Return number of operations as function result. */
static int
parse_ops(char *arg, struct sembuf sops[])
{
    char *comma, *sign, *remaining, *flags;
    int num_ops;        /* Number of operations in 'arg' */

    for (num_ops = 0, remaining = arg; /* do nothing */; ++num_ops) {
        if (num_ops >= MAX_SEMOPS) {
            fprintf(stderr,
                    "Too many operations (maximum=%d): \"%s\"\n",
                    MAX_SEMOPS, arg);
            exit(EXIT_FAILURE);
        }

        if (*remaining == '\0') {
            fprintf(stderr,
                    "Trailing comma or empty argument: \"%s\"\n", arg);
            exit(EXIT_FAILURE);
        }
        if (!isdigit((unsigned char) *remaining)) {
            fprintf(stderr,
                    "Expected initial digit: \"%s\"\n", arg);
            exit(EXIT_FAILURE);
        }

        sops[num_ops].sem_num = strtol(remaining, &sign, 10);
        if (*sign == '\0' || strchr("+-=", *sign) == NULL) {
            fprintf(stderr,
                    "Expected '+', '-', or '=' in \"%s\"\n", arg);
            exit(EXIT_FAILURE);
        }
        if (!isdigit((unsigned char) *(sign + 1))) {
            fprintf(stderr,
                    "Expected digit after '%c' in \"%s\"\n", *sign, arg);
            exit(EXIT_FAILURE);
        }

        sops[num_ops].sem_op = strtol(sign + 1, &flags, 10);
        if (*sign == '-') {
            /* Reverse sign of operation */
            sops[num_ops].sem_op = - sops[num_ops].sem_op;
        } else if (*sign == '=') {
            /* Should be '=0' */
            if (sops[num_ops].sem_op != 0) {
                fprintf(stderr,
                        "Expected \"=0\" in \"%s\"\n", arg);
                exit(EXIT_FAILURE);
            }
        }

        sops[num_ops].sem_flg = 0;
        for (/* do nothing */; /*do nothing */; flags++) {
            if (*flags == 'n') {
                sops[num_ops].sem_flg |= IPC_NOWAIT;
            } else if (*flags == 'u') {
                sops[num_ops].sem_flg |= SEM_UNDO;
            } else {
                break;
            }
        }

        if (*flags != ',' && *flags != '\0') {
            fprintf(stderr,
                    "Bad trailing character (%c) in \"%s\"\n",
                    *flags, arg);
            exit(EXIT_FAILURE);
        }
        comma = strchr(remaining, ',');
        if (comma == NULL) {
            /* No comma --> no more ops */
            break;
        } else {
            remaining = comma + 1;
        }
    }

    return num_ops + 1;
}


static char *
now(char const *format)
{
    time_t t;
    struct tm tm;
    static char buf[1024];

    if (time(&t) == -1) {
        return NULL;
    }

    if (localtime_r(&t, &tm) == NULL) {
        return NULL;
    }

    errno = 0;
    if (strftime(buf, 1024, format == NULL ? "%T" : format, &tm) == 0 &&
            errno != 0) {
        return NULL;
    }

    return buf;
}


int
main(int argc, char *argv[])
{
    struct sembuf sops[MAX_SEMOPS];
    int ind, nsops;

    if (argc < 2 || strcmp(argv[1], "--help") == 0) {
        usage(stderr, argv[0]);
        exit(EXIT_FAILURE);
    }

    for (ind = 2; argv[ind] != NULL; ++ind) {
        nsops = parse_ops(argv[ind], sops);
        printf("nspops: %d\n", nsops);

        printf("%5ld, %s: about to semop()  [%s]\n",
                (long) getpid(), now("%T"), argv[ind]);

        if (semop(atoi(argv[1]), sops, nsops) == -1) {
            fprintf(stderr, "semop (PID=%ld): %s\n",
                    (long) getpid(), strerror(errno));
            exit(EXIT_FAILURE);
        }

        printf("%5ld, %s: semop() completed [%s]\n",
                (long) getpid(), now("%T"), argv[ind]);
    }

    exit(EXIT_SUCCESS);
}
