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


/* pipe_sync.c

   Show how pipes can be used for synchronizing the actions of a parent and
   multiple child processes.

   Usage: pipe_sync sleep-time...

   After creating a pipe, the program creates one child for each command-line
   argument. Each child simulates doing some work by sleeping for the number of
   seconds specified in the corresponding command-line argument. When it has
   finished doing its "work", each child closes its file descriptor for the
   write end of the pipe; the parent can see that all children have finished
   their work when it sees end-of-file on the read end of the pipe.
*/


#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>


static char *
now(char const *format)
{
#define BUF_SIZE 1024
    static char buf[BUF_SIZE];
    time_t t;
    size_t s;
    struct tm *tm;

    t = time(NULL);
    tm = localtime(&t);
    if (tm == NULL) {
        return NULL;
    }

    s = strftime(buf, BUF_SIZE, (format != NULL) ? format : "%c", tm);

    return (s == 0) ? NULL : buf;
}


int
main(int argc, char *argv[])
{
    int pfd[2];         /* Process synchronization pipe */
    int j, dummy;
    int sleep_time;

    if (argc < 2 || strcmp(argv[1], "--help") == 0) {
        fprintf(stderr, "usage: %s sleep-time...\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    /* Make stdout unbuffered, since we terminate child with _exit() */
    setbuf(stdout, NULL);

    printf("%s  Parent started\n", now("%T"));

    if (pipe(pfd) == -1) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }

    for (j = 1; j < argc; ++j) {
        switch (fork()) {
        case -1:
            perror("close");
            exit(EXIT_FAILURE);

        case 0:
            /* Child */
            /* Read end is unused */
            if (close(pfd[0]) == -1) {
                perror("close");
                exit(EXIT_FAILURE);
            }

            /* Child does some work, and lets parent know it's done */
            sleep_time = atoi(argv[j]);
            if (sleep_time <= 0) {
                fprintf(stderr, "sleep-time %d > 0\n", sleep_time);
                exit(EXIT_FAILURE);
            }
            sleep(sleep_time);

            printf("%s  Child %d (PID=%ld) closing pipe\n",
                    now("%T"), j, (long) getpid());
            if (close(pfd[1]) == -1) {
                perror("close");
                exit(EXIT_FAILURE);
            }

            /* Child now carries on to do other things... */
            _exit(EXIT_SUCCESS);

        default: /* Parent loops to create next child */
            break;
        }
    }

    /* Parent comes here; close write end of pipe so we can see EOF */
    /* Write end is unused */
    if (close(pfd[1]) == -1) {
        perror("close");
        exit(EXIT_FAILURE);
    }

    if (read(pfd[0], &dummy, 1) != 0) {
        fprintf(stderr, "parent didn't get EOF\n");
        exit(EXIT_FAILURE);
    }
    printf("%s  Parent ready to go\n", now("%T"));

    /* Parent can now carry on to do other things... */

    exit(EXIT_SUCCESS);
}
