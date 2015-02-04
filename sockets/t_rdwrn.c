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
#include <string.h>

#include "rdwrn.h"

#define BUF_SIZE 32


int
main(int argc __attribute__((unused)),
     char *argv[] __attribute__((unused)))
{
    char buf[BUF_SIZE];
    ssize_t bytes;

    bytes = readn(STDIN_FILENO, buf, BUF_SIZE);
    if (bytes == -1) {
        perror("readn");
        exit(EXIT_FAILURE);
    }

    printf("---\n");

    bytes = writen(STDOUT_FILENO, buf, BUF_SIZE);
    if (bytes == -1) {
        perror("writen");
        exit(EXIT_FAILURE);
    }

    exit(EXIT_SUCCESS);
}
