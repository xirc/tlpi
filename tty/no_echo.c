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


/* no_echo.c

   Demonstrate the use of tcgetattr() and tcsetattr() to change terminal
   attributes. In this case, we disable echoing of terminal input.
*/


#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <termios.h>

#define BUF_SIZE 128


int
main(int argc __attribute__((unused)),
     char *argv[] __attribute__((unused)))
{
    struct termios tp, save;
    char buf[BUF_SIZE];

    /* Retrieve current terminal settings, turn echoing off */
    if (tcgetattr(STDIN_FILENO, &tp) == -1) {
        perror("tcgetattr");
        exit(EXIT_FAILURE);
    }
    save = tp;              /* So we can restore settings later; */
    tp.c_lflag &= ~ECHO;    /* ECHO off, other bits unchanged */
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &tp) == -1) {
        perror("tcsetattr");
        exit(EXIT_FAILURE);
    }

    /* Read some input and then display it back to the user */
    printf("Enter text: ");
    fflush(stdout);
    if (fgets(buf, BUF_SIZE, stdin) == NULL) {
        printf("Got end-of-file/error on fgets()\n");
    } else {
        printf("\nRead: %s", buf);
    }

    /* Restore original terminal settings */
    if (tcsetattr(STDIN_FILENO, TCSANOW, &save) == -1) {
        perror("tcsetattr");
        exit(EXIT_FAILURE);
    }

    exit(EXIT_SUCCESS);
}
