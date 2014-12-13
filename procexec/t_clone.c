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


/* t_clone.c

   Demonstrate the use of the Linux-specific clone() system call.
*/


#define _GNU_SOURCE
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <sched.h>
#include <string.h>


#ifndef CHILD_SIG
#define CHILD_SIG SIGUSR1
    /* Signal to be generated on termination of cloned child */
#endif


/* Startup function for cloned child */
static int
child_func(void *arg)
{
    if (close(*((int *) arg)) == -1) {
        perror("close");
        exit(EXIT_FAILURE);
    }
    return 0; /* Child terminates now */
}


int
main(int argc,
     char *argv[] __attribute__((unused)))
{
    const int STACK_SIZE = 0x10000;   /* Stack size for cloned child */
    char *stack;                      /* Start of stack buffer */
    char *stack_top;                  /* End of stack buffer */
    int s, fd, flags;

    fd = open("/dev/null", O_RDWR);   /* Child will close this fd */
    if (fd == -1) {
        perror("open");
        exit(EXIT_FAILURE);
    }

    /* If argc > 1, child shares file descriptor table with parent */
    flags = (argc > 1) ? CLONE_FILES : 0;

    /* Allocate stack for child */
    stack = malloc(STACK_SIZE);
    if (stack == NULL) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }
    stack_top = stack + STACK_SIZE;     /* Assume stack grows downward */

    /* Ignore CHILD_SIG, in case it is a signal whose default is to
     * terminate the process; but don't ignore SIGCHLD (which is ignored
     * by default), since that would prevent the creation of a zombie. */
    if (CHILD_SIG != 0 && CHILD_SIG != SIGCHLD) {
        if (signal(CHILD_SIG, SIG_IGN) == SIG_ERR) {
            perror("signal");
            exit(EXIT_FAILURE);
        }
    }

    /* Create child; child commences execution in child_func() */
    if (clone(child_func, stack_top, flags | CHILD_SIG, (void*) &fd) == -1) {
        perror("clone");
        exit(EXIT_FAILURE);
    }

    /* Parent falls through to here. Wait for child; __WCLONE is
     * needed for child notifying with signal other than SIGCHLD. */
    if (waitpid(-1, NULL, (CHILD_SIG != SIGCHLD) ? __WCLONE : 0) == -1) {
        perror("waitpid");
        exit(EXIT_FAILURE);
    }
    printf("child has terminated.\n");

    /* Did close() of file descriptor in child affect parent? */
    s = write(fd, "x", 1);
    if (s == -1 && errno == EBADF) {
        printf("file descriptor %d has been closed\n", fd);
    } else if (s == -1) {
        printf("write() on file descriptor %d failed "
                "unexpectedly (%s)\n", fd, strerror(errno));
    } else {
        printf("write() on file descriptor %d succeeded\n", fd);
    }

    exit(EXIT_SUCCESS);
}
