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
#include <stdio.h>
#include <stdlib.h>

#include "readline2.h"


#define RLBUF_SIZE 10
#define BUF_SIZE 20


int
main(int argc __attribute__((unused)),
     char *argv[] __attribute__((unused)))
{
    struct rlbuf rlbuf;
    char buf[BUF_SIZE];
    ssize_t num_read;

    if (readline_init(&rlbuf, STDIN_FILENO, RLBUF_SIZE) == -1) {
        perror("readline_init");
        exit(EXIT_FAILURE);
    }

    while (1) {
        num_read = readline(&rlbuf, buf, BUF_SIZE);
        if (num_read == 0) {
            break;
        }
        if (num_read == -1) {
            perror("readline");
            exit(EXIT_FAILURE);
        }
        if (buf[num_read-1] == '\n') {
            printf("read %3ld bytes, \"%.*s\\n\"\n",
                    (long) num_read, (int) num_read - 1, buf);
        } else {
            printf("read %3ld bytes, \"%.*s\"\n",
                    (long) num_read, (int) num_read, buf);
            printf("    some bytes dropped\n");
        }
    }

    if (readline_free(&rlbuf) == -1) {
        perror("readline_free");
        exit(EXIT_FAILURE);
    }

    exit(EXIT_SUCCESS);
}
