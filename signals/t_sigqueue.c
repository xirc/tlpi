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


/* t_sigqueue.c

   Demonstrate the use of sigqueue() to send a (realtime) signal.

   Usage: t_sigqueue sig pid data num-sigs

   Send 'num-sigs' instances of the signal 'sig' (specified as an integer), with
   accompanying data 'data' (an integer), to the process with the PID 'pid'.
*/


#define _POSIX_C_SOURCE 199309
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <string.h>


int
main(int argc, char *argv[])
{
    int pid, sig, num_sigs, sig_data, j;
    union sigval sv;

    if (argc < 4 || strcmp(argv[1], "--help") == 0) {
        fprintf(stderr,
                "usage: %s pid sig-num data [num-sigs]\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    /* Display our PID and UID, so that they can be compared with the
     * corresponding fields of the siginfo_t argument supplied to the
     * handle in the receiving process
     */
    printf("%s: PID is %ld, UID is %ld\n", argv[0],
            (long) getpid(), (long) getuid());

    pid = atoi(argv[1]);
    sig = atoi(argv[2]);
    sig_data = atoi(argv[3]);
    num_sigs = (argc > 4) ? atoi(argv[4]) : 1;
    if (num_sigs < 0) {
        fprintf(stderr, "num-sigs %s > 0\n", argv[4]);
        exit(EXIT_FAILURE);
    }

    for (j = 0; j < num_sigs; ++j) {
        sv.sival_int = sig_data + j;
        if (sigqueue(pid, sig, sv) == -1) {
            perror("sigqueue");
            exit(EXIT_FAILURE);
        }
    }

    exit(EXIT_SUCCESS);
}
