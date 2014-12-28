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


/* t_getservbyname.c

   Demonstrate the use of getservbyname() to look up the port number
   corresponding to a given service name. Note that getservbyname() is
   now obsolete; new programs should use getaddrinfo().
*/


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
main(int argc,
     char *argv[])
{
    char *name, *proto;
    struct servent *ent;
    char **pp;

    if (argc < 2 || strcmp(argv[0], "--help") == 0) {
        fprintf(stderr, "Usage: %s service [protocol]\n", argv[0]);
        fprintf(stderr, "    service: service name\n");
        fprintf(stderr, "    protocol: udp or tcp\n");
        exit(EXIT_FAILURE);
    }
    name = argv[1];
    proto = NULL;   /* default value */
    if (argc == 3) {
        if (strcmp(argv[2], "udp") == 0 || strcmp(argv[2], "tcp") == 0) {
            proto = argv[2];
        } else {
            fprintf(stderr, "INVALID protocol %s\n", argv[2]);
            exit(EXIT_FAILURE);
        }
    }

    for (argv++; *argv != NULL; argv++) {
        ent = getservbyname(name, proto);
        if (ent == NULL) {
            fprintf(stderr, "getservbyname() failed for '%s': %s\n",
                    *argv, hstrerror(h_errno));
            continue;
        }

        printf("Service name: %s\n", ent->s_name);

        printf("        alias(es):      ");
        for (pp = ent->s_aliases; *pp != NULL; ++pp) {
            printf(" %s", *pp);
        }
        printf("\n");

        printf("        port:            %d\n", ntohs(ent->s_port));
        printf("        protocol:        %s\n", ent->s_proto);
    }

    exit(EXIT_SUCCESS);
}
