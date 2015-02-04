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


/* test_tty_functions.c

   Put the terminal in raw mode (or cbreak if a command-line argument is
   supplied), allowing the program to read input a character at a time, and
   read and echo characters.

   Usage: test_tty_functions [x]

   See also tty_functions.c.
*/


#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <termios.h>
#include <signal.h>
#include <ctype.h>

#include "tty_functions.h"


static struct termios user_termios;
        /* Terminal settings as defined by user */


/* General handler: restore tty settings and exit */
static void
handler(int sig __attribute__((unused)))
{
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &user_termios) == -1) {
        perror("tcsetattr");
        exit(EXIT_FAILURE);
    }
    _exit(EXIT_SUCCESS);
}


/* Handler for SIGTSTP */
static void
tstp_handler(int sig __attribute__((unused)))
{
    struct termios our_termios;     /* To save our tty settings */
    sigset_t tstp_mask, prev_mask;
    struct sigaction sa;
    int saved_errno;

    saved_errno = errno;        /* We might change 'errno' here */

    /* Save current terminal settings, restore terminal to
     * state at time of program startup */
    if (tcgetattr(STDIN_FILENO, &our_termios) == -1) {
        perror("tcgetattr");
        exit(EXIT_FAILURE);
    }
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &user_termios) == -1) {
        perror("tcsetattr");
        exit(EXIT_FAILURE);
    }

    /* Set the disposition of SIGTSTP to the default, raise the signal
     * once more, and then unblock it so that we actually stop */
    if (signal(SIGTSTP, SIG_DFL) == SIG_ERR) {
        perror("signal");
        exit(EXIT_FAILURE);
    }
    raise(SIGTSTP);

    sigemptyset(&tstp_mask);
    sigaddset(&tstp_mask, SIGTSTP);
    if (sigprocmask(SIG_UNBLOCK, &tstp_mask, &prev_mask) == -1) {
        perror("sigprocmask");
        exit(EXIT_FAILURE);
    }

    /* Execution resumes here after SIGCONT */
    /* Reblock SIGTSTP */
    if (sigprocmask(SIG_SETMASK, &prev_mask, NULL) == -1) {
        perror("sigprocmask");
        exit(EXIT_FAILURE);
    }

    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    sa.sa_handler = tstp_handler;
    if (sigaction(SIGTSTP, &sa, NULL) == -1) {
        perror("sigaction");
        exit(EXIT_FAILURE);
    }

    /* The user may have changed the terminal settings while we were
     * stopped; save the settings so we can restore them later */
    if (tcgetattr(STDIN_FILENO, &user_termios) == -1) {
        perror("tcgetattr");
        exit(EXIT_FAILURE);
    }

    /* Restore our terminal settings */
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &our_termios) == -1) {
        perror("tcsetattr");
        exit(EXIT_FAILURE);
    }

    errno = saved_errno;
}


int
main(int argc, char *argv[] __attribute__((unused)))
{
    char ch;
    struct sigaction sa, prev;
    ssize_t n;

    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;

    if (argc > 1) {     /* Use cbreak mode */
        if (tty_set_cbreak(STDIN_FILENO, &user_termios) == -1) {
            perror("tty_set_cbreak");
            exit(EXIT_FAILURE);
        }

        /* Terminal special characters can generate signals in cbreak mode.
         * Catch them so that we can adjust the terminal mode.
         * We establish handlers only if the signals are not being ignored. */
        sa.sa_handler = handler;
        if (sigaction(SIGQUIT, NULL, &prev) == -1) {
            perror("sigaction");
            exit(EXIT_FAILURE);
        }
        if (prev.sa_handler != SIG_IGN) {
            if (sigaction(SIGQUIT, &sa, NULL) == -1) {
                perror("sigaction");
                exit(EXIT_FAILURE);
            }
        }

        if (sigaction(SIGINT, NULL, &prev) == -1) {
            perror("sigaction");
            exit(EXIT_FAILURE);
        }
        if (prev.sa_handler != SIG_IGN) {
            if (sigaction(SIGINT, &sa, NULL) == -1) {
                perror("sigaction");
                exit(EXIT_FAILURE);
            }
        }

        sa.sa_handler = tstp_handler;
        if (sigaction(SIGTSTP, NULL, &prev) == -1) {
            perror("sigaction");
            exit(EXIT_FAILURE);
        }
        if (prev.sa_handler != SIG_IGN) {
            if (sigaction(SIGTSTP, &sa, NULL) == -1) {
                perror("sigaction");
                exit(EXIT_FAILURE);
            }
        }
    } else {        /* Use raw mode */
        if (tty_set_raw(STDIN_FILENO, &user_termios) == -1) {
            perror("tty_set_raw");
            exit(EXIT_FAILURE);
        }
    }

    sa.sa_handler = handler;
    if (sigaction(SIGTERM, &sa, NULL) == -1) {
        perror("sigaction");
        exit(EXIT_FAILURE);
    }

    /* Disable stdout buffering */
    setbuf(stdout, NULL);

    /* Read and echo stdin */
    while (1) {
        n = read(STDIN_FILENO, &ch, 1);
        if (n == -1) {
            fprintf(stderr, "read\n");
            break;
        }
        /* Can occur after terminal disconnect */
        if (n == 0) {
            break;
        }

        if (isalpha((unsigned char) ch)) {
            /* Letters --> lowercase */
            putchar(tolower((unsigned char) ch));
        } else if (ch == '\n' || ch == '\r') {
            putchar(ch);
        } else if (iscntrl((unsigned char) ch)) {
            printf("^%c", ch ^ 64);     /* Echo Control-A as ^A, etc. */
        } else {
            putchar('*');               /* All other chars as '*' */
        }

        if (ch == 'q') {                /* Quit loop */
            break;
        }
    }

    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &user_termios) == -1) {
        perror("tcsetattr");
        exit(EXIT_FAILURE);
    }

    exit(EXIT_SUCCESS);
}
