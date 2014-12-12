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


/* system.c

   An implementation of system(3).
*/


#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <sys/wait.h>

#include "print_wait_status.h"


static int
m_system(char const *command)
{
    sigset_t block_mask, orig_mask;
    struct sigaction sa_ignore, sa_orig_quit, sa_orig_int, sa_default;
    pid_t cpid;
    int status, saved_errno;

    if (command == NULL) {
        /* Is a shell available? */
        return m_system(":") == 0;
    }

    sigemptyset(&block_mask);
    sigaddset(&block_mask, SIGCHLD);
    sigprocmask(SIG_BLOCK, &block_mask, &orig_mask);

    sa_ignore.sa_handler = SIG_IGN;
    sa_ignore.sa_flags = 0;
    sigemptyset(&sa_ignore.sa_mask);
    sigaction(SIGINT, &sa_ignore, &sa_orig_int);
    sigaction(SIGQUIT, &sa_ignore, &sa_orig_quit);

    switch (cpid = fork()) {
    case -1: /* fork() failed */
        status = -1;
        break;
            /* Carry on to reset signal attributes */

    case 0: /*Child: exec command */
        sa_default.sa_handler = SIG_DFL;
        sa_default.sa_flags = 0;
        sigemptyset(&sa_default.sa_mask);

        if (sa_orig_int.sa_handler != SIG_IGN) {
            sigaction(SIGINT, &sa_default, NULL);
        }
        if (sa_orig_quit.sa_handler != SIG_IGN) {
            sigaction(SIGQUIT, &sa_default, NULL);
        }

        sigprocmask(SIG_SETMASK, &orig_mask, NULL);

        execl("/bin/sh", "sh", "-c", command, (char *) NULL);
        _exit(127);     /* We could not exec the shell */

    default: /* Parent: wait for our child to terminate */
        while (waitpid(cpid, &status, 0) == -1) {
            if (errno != EINTR) {
                status = -1;
                break;
            }
        }
        break;
    }

    /* Unblock SIGCHLD, restore dispositions of SIGINT and SIGQUIT */
    saved_errno = errno;
        /* The following may change 'errno' */
    sigprocmask(SIG_SETMASK, &orig_mask, NULL);
    sigaction(SIGINT, &sa_orig_int, NULL);
    sigaction(SIGQUIT, &sa_orig_quit, NULL);

    errno = saved_errno;

    return status;
}


#define MAX_CMD_LEN 256
int
main(int argc __attribute__((unused)),
     char *argv[] __attribute__((unused)))
{
    /* Command to be executed by m_system() */
    char str[MAX_CMD_LEN];
    /* Status return from m_system() */
    int status;

    while (1) {
        printf("Command: ");
        fflush(stdout);
        if (fgets(str, MAX_CMD_LEN, stdin) == NULL) {
            break; /* end-of-file */
        }

        status = m_system(str);
        printf("m_system() returned: status=0x%04x (%d,%d)\n",
                (unsigned int) status, status >> 8, status & 0xFF);

        if (status == -1) {
            perror("m_system");
            exit(EXIT_FAILURE);
        } else {
            if (WIFEXITED(status) && WEXITSTATUS(status) == 127) {
                printf("(Probably) could not invoke shell\n");
            } else {
                print_wait_status(NULL, status);
            }
        }
    }

    exit(EXIT_SUCCESS);
}
