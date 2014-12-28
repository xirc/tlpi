/*************************************************************************\
*                  Copyright (C) xirc, 2014.                              *
*                                                                         *
* This program is free software. You may use, modify, and redistribute it *
* under the terms of the GNU Affero General Public License as published   *
* by the Free Software Foundation, either version 3 or (at your option)   *
* any later version. This program is distributed without any warranty.    *
* See the file COPYING.agpl-v3 for details.                               *
\*************************************************************************/


#define _BSD_SOURCE
    /* To get hstrerror() declaration from <netdb.h> */
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>


int
main(int argc __attribute__((unused)),
     char *argv[])
{
    struct in_addr addr;
    struct hostent *ent;
    char **pp;
    char str[INET6_ADDRSTRLEN];

    for (argv++; *argv != NULL; argv++) {
        if (inet_aton(*argv, &addr) == 0) {
            fprintf(stderr, "inet_aton() failed for '%s'\n", *argv);
            continue;
        }
        ent = gethostbyaddr(&addr, sizeof(struct in_addr), AF_INET);
        if (ent == NULL) {
            fprintf(stderr, "gethostbyaddr() failed for '%s': %s\n",
                    *argv, hstrerror(h_errno));
            continue;
        }

        printf("Canonical name: %s\n", ent->h_name);

        printf("        alias(es):      ");
        for (pp = ent->h_aliases; *pp != NULL; ++pp) {
            printf(" %s", *pp);
        }
        printf("\n");

        printf("        address type:   %s\n",
                (ent->h_addrtype == AF_INET) ? "AF_INET" :
                (ent->h_addrtype == AF_INET6) ? "AF_INET6" : "???");

        if (ent->h_addrtype == AF_INET || ent->h_addrtype == AF_INET6) {
            printf("          address(es):   ");
            for (pp = ent->h_addr_list; *pp != NULL; ++pp) {
                printf(" %s", inet_ntop(ent->h_addrtype, *pp,
                            str, INET6_ADDRSTRLEN));
            }
            printf("\n");
        }
    }

    exit(EXIT_SUCCESS);
}
