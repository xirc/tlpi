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
#include <termios.h>


int
main(int argc __attribute__((unused)),
     char *argv[] __attribute__((unused)))
{
    struct termios to;
    int is_canonnical;

    if (isatty(STDIN_FILENO) == 0) {
        perror("isatty");
        exit(EXIT_FAILURE);
    }

    if (tcgetattr(STDIN_FILENO, &to) == -1) {
        perror("tcgetattr");
        exit(EXIT_FAILURE);
    }

    is_canonnical = to.c_lflag & ICANON;
    printf("%s\n", is_canonnical ? "CANONICAL" : "NONCANONICAL");
    if (is_canonnical) {
        printf("TIME: %d\n", to.c_cc[VTIME]);
        printf("MIN:  %d\n", to.c_cc[VMIN]);
    }

    exit(EXIT_SUCCESS);
}
