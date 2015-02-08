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


/* demo_sigio.c

   A trivial example of the use of signal-driven I/O.
*/


#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <ctype.h>
#include <fcntl.h>
#include <termios.h>

#include "tty_functions.h"


static volatile sig_atomic_t got_sigio = 0;


static void
sigio_handler(int sig __attribute__((unused)))
{
    got_sigio = 1;
}


int
main(int argc __attribute__((unused)),
     char *argv[] __attribute__((unused)))
{
    int flags, j, cnt;
    struct termios orig_termios;
    char ch;
    struct sigaction sa;
    int done;

    /* Establish handler for "I/O possible" signal */
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    sa.sa_handler = sigio_handler;
    if (sigaction(SIGIO, &sa, NULL) == -1) {
        perror("sigaction");
        exit(EXIT_FAILURE);
    }

    /* Set owner process that is to receive "I/O possible" signal */
    if (fcntl(STDIN_FILENO, F_SETOWN, getpid()) == -1) {
        perror("fcntl(F_SETOWN)");
        exit(EXIT_FAILURE);
    }

    /* Enable "I/O possible" signaling and
     * make I/O nonblocking for file descriptor */
    flags = fcntl(STDIN_FILENO, F_GETFL);
    if (fcntl(STDIN_FILENO, F_SETFL, flags | O_ASYNC | O_NONBLOCK) == -1) {
        perror("fcntl(F_SETFL)");
        exit(EXIT_FAILURE);
    }

    /* Place terminal in cbreak mode */
    if (tty_set_cbreak(STDIN_FILENO, &orig_termios) == -1) {
        perror("tty_set_cbreak");
        exit(EXIT_FAILURE);
    }

    for (done = 0, cnt = 0; !done; ++cnt) {
        for (j = 0; j < 100000000; ++j) {
            continue;
                /* Slow main loop down a little */
        }

        if (got_sigio) {    /* Is input available ? */
            got_sigio = 0;

            /* Read all available input until error (possibly EAGAIN)
             * or EOF (not actually possible in cbreak mode) or
             * a hash (#) character is read */
            while (read(STDIN_FILENO, &ch, 1) > 0 && !done) {
                printf("cnt=%d; read %c\n", cnt, ch);
                done = (ch == '#');
            }
        }
    }

    /* Restore original terminal settings */
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios) == -1) {
        perror("tcsetattr");
        exit(EXIT_FAILURE);
    }

    exit(EXIT_SUCCESS);
}
