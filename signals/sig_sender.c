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


/* sig_sender.c

   Usage: sig_sender PID num-sigs sig [sig2]

   Send signals to sig_receiver.c.

   Sends 'num-sigs' signals of type 'sig' to the process with the specified PID.
   If a fourth command-line argument is supplied, send one instance of that
   signal, after sending the previous signals.
*/


#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>


int
main(int argc, char *argv[])
{
    int num_sigs, sig, sig2, j;
    pid_t pid;

    if (argc < 4 || strcmp(argv[1], "--help") == 0) {
        fprintf(stderr, "%s pid num-sigs sig-num [sig-num-2]\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    pid = atoi(argv[1]);
    num_sigs = atoi(argv[2]);
    if (num_sigs < 0) {
        fprintf(stderr, "num-sigs %s > 0\n", argv[2]);
        exit(EXIT_FAILURE);
    }
    sig = atoi(argv[3]);
    if (argc > 4) {
        sig2 = atoi(argv[4]);
    }

    /* send signals to receiver */
    printf("%s: sending signal %d to process %ld %d times\n",
            argv[0], sig, (long) pid, num_sigs);
    for (j = 0; j < num_sigs; ++j) {
        if (kill(pid, sig) == -1) {
            perror("kill");
            exit(EXIT_FAILURE);
        }
    }
    /* if a fourth command-line argument was specified, send that signal */
    if (argc > 4) {
        if (kill(pid, sig2) == -1) {
            perror("kill");
            exit(EXIT_FAILURE);
        }
    }
    printf("%s: existing\n", argv[0]);
    exit(EXIT_SUCCESS);
}
