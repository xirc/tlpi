/*************************************************************************\
*                  Copyright (C) xirc, 2014.                              *
*                                                                         *
* This program is free software. You may use, modify, and redistribute it *
* under the terms of the GNU Affero General Public License as published   *
* by the Free Software Foundation, either version 3 or (at your option)   *
* any later version. This program is distributed without any warranty.    *
* See the file COPYING.agpl-v3 for details.                               *
\*************************************************************************/


#define _GNU_SOURCE
    /* Enable inet_aton() and inet_ntoa() */

#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>


int
main(int argc, char *argv[])
{
    int s;
    struct in_addr addr;
    char *addrstr;

    if (argc < 2 || strcmp(argv[0], "--help") == 0) {
        fprintf(stderr, "Usage: %s ip\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    s = inet_aton(argv[1], &addr);
    if (s == 0) {
        fprintf(stderr, "Invalid hostname\n");
        exit(EXIT_FAILURE);
    }

    addrstr = inet_ntoa(addr);
    printf("%s\n", addrstr);

    exit(EXIT_SUCCESS);
}
