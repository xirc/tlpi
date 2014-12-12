/*************************************************************************\
*                  Copyright (C) xirc, 2014.                              *
*                                                                         *
* This program is free software. You may use, modify, and redistribute it *
* under the terms of the GNU Affero General Public License as published   *
* by the Free Software Foundation, either version 3 or (at your option)   *
* any later version. This program is distributed without any warranty.    *
* See the file COPYING.agpl-v3 for details.                               *
\*************************************************************************/


#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <sys/wait.h>


static void
handler(int sig __attribute__((unused)))
{
    /* do nothing */
}


int
main(int argc __attribute__((unused)),
     char *argv[] __attribute__((unused)))
{
    pid_t pid, cpid, gcpid;
    sigset_t block_mask, empty_mask;
    struct sigaction sa;

    /* Disable buffering of stdout */
    setbuf(stdout, NULL);

    /* Set signal SIGUSR1 handler */
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sa.sa_handler = handler;
    if (sigaction(SIGUSR1, &sa, NULL) == -1) {
        perror("sigaction");
        exit(EXIT_FAILURE);
    }

    /* Block signal SIGUSR1 */
    sigemptyset(&block_mask);
    sigaddset(&block_mask, SIGUSR1);
    if (sigprocmask(SIG_SETMASK, &block_mask, NULL) == -1) {
        perror("sigprocmask");
        exit(EXIT_FAILURE);
    }

    pid = getpid();
    switch (cpid = fork()) {
    case -1:
        perror("fork");
        exit(EXIT_FAILURE);

    case 0:
        switch (gcpid = fork()) {
        case -1:
            perror("fork2");
            exit(EXIT_FAILURE);

        case 0:
            /* grandchild */
            printf("Grandchild: ppid = %ld\n", (long) getppid());
            if (kill(getppid(), SIGUSR1) == -1) {
                perror("kill");
                exit(EXIT_FAILURE);
            }
            sleep(3); /* Give child a chance to exit */
            printf("Grandchild: ppid = %ld\n", (long) getppid());
            if (kill(pid, SIGUSR1) == -1) {
                perror("kill");
                exit(EXIT_FAILURE);
            }
            sleep(3); /* Give parent a chance to wait */
            printf("Grandchild: ppid = %ld\n", (long) getppid());
            if (kill(pid, SIGUSR1) == -1) {
                perror("kill");
                exit(EXIT_FAILURE);
            }
            _exit(EXIT_SUCCESS);

        default:
            /* child */
            printf("Child: ppid = %ld\n", (long) getppid());

            sigemptyset(&empty_mask);
            if (sigsuspend(&empty_mask) == -1 && errno != EINTR) {
                perror("sigsuspend");
                exit(EXIT_FAILURE);
            }
            if (kill(getppid(), SIGUSR1) == -1) {
                perror("kill");
                exit(EXIT_FAILURE);
            }
            printf("Child: exiting\n");
            _exit(EXIT_SUCCESS);
        }

    default:
        /* parent */
        sigemptyset(&empty_mask);
        if (sigsuspend(&empty_mask) == -1 && errno != EINTR) {
            perror("sigsuspend");
            exit(EXIT_FAILURE);
        }
        if (sigsuspend(&empty_mask) == -1 && errno != EINTR) {
            perror("sigsuspend");
            exit(EXIT_FAILURE);
        }
        printf("Parent: deal zombie child\n");
        if (wait(NULL) == -1) {
            perror("wait");
            exit(EXIT_FAILURE);
        }
        if (sigsuspend(&empty_mask) == -1 && errno != EINTR) {
            perror("sigsuspend");
            exit(EXIT_FAILURE);
        }
        printf("Parent: exiting\n");
    }
}
