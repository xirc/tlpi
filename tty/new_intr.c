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


/* new_intr.c

   Demonstrate the use of tcgetattr() and tcsetattr() to change the
   terminal INTR (interrupt) character.
*/


#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <termios.h>
#include <ctype.h>


int
main(int argc, char *argv[])
{
    struct termios tp;
    int intr_char;

    if (argc > 1 && strcmp(argv[1], "--help") == 0) {
        fprintf(stderr, "%s [intr-char]\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    /* Determine new INTR setting from command line */
    if (argc == 1) {        /* Disable */
        intr_char = fpathconf(STDIN_FILENO, _PC_VDISABLE);
        if (intr_char == -1) {
            perror("fpathconf - Couldn't determine VDISABLE");
            exit(EXIT_FAILURE);
        }
    } else if (isdigit((unsigned char) argv[1][0])) {
        /* Allows number, including hex and octal format */
        intr_char = strtoul(argv[1], NULL, 0);
    } else {
        /* Literal character */
        intr_char = argv[1][0];
    }

    /* Fetch current terminal settings, modify INTR character, and
     * push changes back to the terminal driver */
    if (tcgetattr(STDIN_FILENO, &tp) == -1) {
        perror("tcgetattr");
        exit(EXIT_FAILURE);
    }
    tp.c_cc[VINTR] = intr_char;
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &tp) == -1) {
        perror("tcsetattr");
        exit(EXIT_FAILURE);
    }

    exit(EXIT_SUCCESS);
}
