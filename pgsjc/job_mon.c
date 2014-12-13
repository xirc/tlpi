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


/* job_mon.c

   This program is useful for:

      - demonstrating the order in which the shell creates the processes in a
        pipeline and how it assigns a process group to these processes, and

      - monitoring some of the job control signals sent to a process (group).

   The program displays its PID, parent PID and process group.

   Try running a pipeline consisting of a series of these commands:

        job_mon | job_mon | job_mon

   This will demonstrate the assignment of process groups and PIDs to
   each process in the pipeline.

   Try running this pipeline under different shells and also in the background
   (pipeline &) to see the effect this has.

   You can also try typing control-C (^C) to demonstrate that this is
   interpreted by the terminal driver as meaning "send a signal to all the jobs
   in the foreground process group".
*/


#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <fcntl.h>


static int cmd_num;     /* Our position in pipeline */


/* Handler for various signals */
static void
handler(int sig)
{
    /* UNSAFE: This handler uses non-async-signal-safe functions
     * (fprintf(), strsignal(); see Section 21.1.2 */

    if (getpid() == getpgrp()) {
        /* If process group leader */
        fprintf(stderr, "Terminal FG process group: %ld\n",
                (long) tcgetpgrp(STDERR_FILENO));
    }
    fprintf(stderr, "Process %ld (%d) received signal %d (%s)\n",
            (long) getpid(), cmd_num, sig, strsignal(sig));

    /* If we catch SIGTSTP, it won't actually stop us.
     * Therefore we raise SIGSTOP so we actually get stopped. */
    if (sig == SIGTSTP) {
        raise(SIGSTOP);
    }
}


int
main(int argc __attribute__((unused)),
     char *argv[] __attribute__((unused)))
{
    struct sigaction sa;

    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    sa.sa_handler = handler;
    if (sigaction(SIGINT, &sa, NULL) == -1) {
        perror("sigaction");
        exit(EXIT_FAILURE);
    }
    if (sigaction(SIGTSTP, &sa, NULL) == -1) {
        perror("sigaction");
        exit(EXIT_FAILURE);
    }
    if (sigaction(SIGCONT, &sa, NULL) == -1) {
        perror("sigaction");
        exit(EXIT_FAILURE);
    }

    /* If stdin is a terminal, this is the first process in pipeline:
     * print a heading and initialize message to be sent down pipe */
    if (isatty(STDIN_FILENO)) {
        fprintf(stderr, "Terminal FG process group: %ld\n",
                (long) tcgetpgrp(STDIN_FILENO));
        fprintf(stderr, "Command   PID  PPID  PGRP   SID\n");
        cmd_num = 0;
    } else {
        if (read(STDIN_FILENO, &cmd_num, sizeof(cmd_num)) <= 0) {
            fprintf(stderr, "read got EOF or error\n");
            exit(EXIT_FAILURE);
        }
    }

    cmd_num++;
    fprintf(stderr, "%4d    %5ld %5ld %5ld %5ld\n",
            cmd_num, (long) getpid(), (long) getppid(),
            (long) getpgrp(), (long) getsid(0));

    /* If not the last process, pass a message to the next process */
    if (!isatty(STDOUT_FILENO)) { /* If not tty, then should be pipe */
        if (write(STDOUT_FILENO, &cmd_num, sizeof(cmd_num)) == -1) {
            perror("write");
            exit(EXIT_FAILURE);
        }
    }

    /* Wait for signals */
    while (1) {
        pause();
    }

    exit(EXIT_SUCCESS);
}
