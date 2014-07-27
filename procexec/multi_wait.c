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


/* multi_wait.c

   Demonstrate the use of wait(2): create multiple children and then wait
   for them all.

   Usage: multi_wait sleep-time...

   One child process is created for each command-line argument. Each child
   sleeps for the number of seconds specified in the corresponding command-line
   argument before exiting. After all children have been created, the parent
   loops, waiting for terminated children, and displaying their PIDs.
*/


#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/wait.h>
#include <time.h>


static char * now(char const *format);


int
main(int argc, char *argv[])
{
    int num_dead;
        /* Number of children so far waited for */
    pid_t child_pid;
        /* PID of waited for child */
    int j, stime;

    if (argc < 2 || strcmp(argv[1], "--help") == 0) {
        fprintf(stderr, "usage: %s sleep-time...\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    /* Disable buffering of stdout */
    setbuf(stdout, NULL);

    /* Create one child for each argument */
    for (j = 1; j < argc; ++j) {
        switch (fork()) {
        case -1:
            perror("fork");
            exit(EXIT_FAILURE);
        case 0:
            /* Child sleeps for a while then exits */
            printf("[%s] child %d started with PID %ld,"
                   " sleeping %s seconds\n",
                   now("%T"), j, (long) getpid(), argv[j]);
            stime = atoi(argv[j]);
            if (stime < 0) {
                fprintf(stderr, "sleep-time[%d] %s >= 0", j-1, argv[j]);
                exit(EXIT_FAILURE);
            }
            sleep(stime);
            _exit(EXIT_SUCCESS);
        default:
            /* Parent just continues around loop */
            break;
        }
    }

    num_dead = 0;
    while (1) {
        /* Parent waits for each child to exit */
        child_pid = wait(NULL);
        if (child_pid == -1) {
            if (errno == ECHILD) {
                printf("No more children - bye!\n");
                exit(EXIT_SUCCESS);
            } else {
                perror("wait");
                exit(EXIT_FAILURE);
            }
        }

        num_dead++;
        printf("[%s] wait() returned child PID %ld (num_dead=%d)\n",
                now("%T"), (long) child_pid, num_dead);
    }
}


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
#undef BUF_SIZE
}
