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
#include <sys/un.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define BUF_SIZE 4096
#define SERVER_SOCK_PATH "/tmp/ud_dgram.server"
#define CLIENT_SOCK_PATH "/tmp/ud_dgram.client"


static void
server(size_t expected_bytes, int sleep_time_s)
{
    struct sockaddr_un svaddr, claddr;
    int sfd;
    size_t total_bytes;
    ssize_t num_bytes;
    socklen_t len;
    char buf[BUF_SIZE];

    sfd = socket(AF_UNIX, SOCK_DGRAM, 0);
    if (sfd == -1) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    if (remove(SERVER_SOCK_PATH) == -1 && errno != ENOENT) {
        perror("remove " SERVER_SOCK_PATH);
        exit(EXIT_FAILURE);
    }

    memset(&svaddr, 0, sizeof(struct sockaddr_un));
    svaddr.sun_family = AF_UNIX;
    strncpy(svaddr.sun_path, SERVER_SOCK_PATH, sizeof(svaddr.sun_path) - 1);

    if (bind(sfd, (struct sockaddr *) &svaddr,
                sizeof(struct sockaddr_un)) == -1)
    {
        perror("bind");
        exit(EXIT_FAILURE);
    }

    total_bytes = 0;
    while (total_bytes < expected_bytes) {
        len = sizeof(struct sockaddr_un);
        num_bytes = recvfrom(sfd, buf, BUF_SIZE, 0,
                (struct sockaddr *) &claddr, &len);
        if (num_bytes == -1) {
            perror("recvfrom");
            exit(EXIT_FAILURE);
        }
        total_bytes += num_bytes;

        printf("Server received %ld bytes from %s\n",
                (long) num_bytes, claddr.sun_path);

        (void) sleep(sleep_time_s);
    }

    (void) remove(svaddr.sun_path);
}


static void
client(size_t expected_bytes, size_t msgsize)
{
    struct sockaddr_un svaddr, claddr;
    int sfd;
    size_t msglen;
    size_t total_bytes;
    ssize_t num_bytes;
    char buf[BUF_SIZE];

    sfd = socket(AF_UNIX, SOCK_DGRAM, 0);
    if (sfd == -1) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    memset(&claddr, 0, sizeof(struct sockaddr_un));
    claddr.sun_family = AF_UNIX;
    strncpy(claddr.sun_path, CLIENT_SOCK_PATH, sizeof(claddr.sun_path) - 1);

    if (bind(sfd, (struct sockaddr *) &claddr,
                sizeof(struct sockaddr_un)) == -1)
    {
        perror("bind");
        exit(EXIT_FAILURE);
    }

    memset(&svaddr, 0, sizeof(struct sockaddr_un));
    svaddr.sun_family = AF_UNIX;
    strncpy(svaddr.sun_path, SERVER_SOCK_PATH, sizeof(svaddr.sun_path) - 1);

    total_bytes = 0;
    while (total_bytes < expected_bytes) {
        memset(buf, total_bytes & 0xFF, BUF_SIZE);
        msglen = msgsize < BUF_SIZE ? msgsize : BUF_SIZE;
        num_bytes = sendto(sfd, buf, msglen, 0,
                (struct sockaddr *) &svaddr, sizeof(struct sockaddr_un));
        if (num_bytes == -1)
        {
            perror("sendto");
            exit(EXIT_FAILURE);
        }
        total_bytes += num_bytes;

        printf("Client sent %ld bytes to %s\n",
                (long) num_bytes, svaddr.sun_path);
    }

    (void) remove(claddr.sun_path);
}


int
main(int argc, char *argv[])
{
    size_t msgsize;
    size_t expected_bytes;
    int sleep_time_s;

    if (argc < 2 || strcmp(argv[1], "--help") == 0) {
        fprintf(stderr, "Usage: %s total-bytes [msgsize] [sleep]\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    expected_bytes = strtoul(argv[1], NULL, 0);
    msgsize = (argc > 2) ? strtoul(argv[2], NULL, 0) : 1;
    sleep_time_s = (argc > 3) ? strtoul(argv[3], NULL, 0) : 0;

    switch (fork()) {
    case -1:
        perror("fork");
        exit(EXIT_FAILURE);
    case 0:
        /* Child behaves as client */
        sleep(1);
        client(expected_bytes, msgsize);
        break;
    default:
        /* Parent behaves as server */
        server(expected_bytes, sleep_time_s);
        (void) wait(NULL);
        break;
    }

    exit(EXIT_SUCCESS);
}
