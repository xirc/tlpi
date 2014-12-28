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
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>


int
main(int argc __attribute__((unused)),
     char *argv[] __attribute__((unused)))
{
    uint16_t host_uint16, net_uint16;
    uint32_t host_uint32, net_uint32;

    host_uint16 = (uint16_t) 0x1234;
    host_uint32 = (uint32_t) 0x12345678;
    net_uint16 = htons(host_uint16);
    net_uint32 = htonl(host_uint32);
    printf("htons, htonl\n");
    printf("host: %8x, net:  %8x\n", host_uint16, net_uint16);
    printf("host: %8x, net:  %8x\n", host_uint32, net_uint32);

    net_uint16 = (uint16_t) 0x1234;
    net_uint32 = (uint32_t) 0x12345678;
    host_uint16 = ntohs(net_uint16);
    host_uint32 = ntohl(net_uint32);
    printf("ntohs, ntohl\n");
    printf("net:  %8x, host: %8x\n", net_uint16, host_uint16);
    printf("net:  %8x, host: %8x\n", net_uint32, host_uint32);

    exit(EXIT_SUCCESS);
}
