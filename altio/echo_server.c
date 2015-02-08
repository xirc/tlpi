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
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <sys/wait.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <signal.h>
#include <ctype.h>

#define TCP_ECHO_SERVICE       7
#define UDP_ECHO_SERVICE       7
#define BACKLOG                5
#define BUF_SIZE            4096


static void
grim_ripper(int sig)
{
    assert(sig == SIGCHLD);

    while (waitpid(-1, NULL, WNOHANG) > 0) {
        continue;
    }
}


static void
serve_tcpecho(int sfd)
{
    int cfd;
    ssize_t num_recv;
    char buf[BUF_SIZE];

    printf("TCP\n");

    cfd = accept(sfd, NULL, NULL);
    if (cfd == -1) {
        perror("accept");
        exit(EXIT_FAILURE);
    }

    switch (fork()) {
    case -1:
        perror("fork");
        exit(EXIT_FAILURE);
    case 0:     /* @child */
        (void) close(sfd);
        break;
    default:    /* @parent */
        (void) close(cfd);
        return;
    }

    /* echo service @child */
    while (1) {
        num_recv = recv(cfd, buf, BUF_SIZE, 0);
        if (num_recv == -1) {
            perror("recv");
            exit(EXIT_FAILURE);
        }
        if (num_recv == 0) {
            break;
        }
        if (send(cfd, buf, num_recv, 0) == -1) {
            perror("send");
            exit(EXIT_FAILURE);
        }
    }
    exit(EXIT_SUCCESS);
}


static void
serve_udpecho(int sfd)
{
    ssize_t num_recv;
    char buf[BUF_SIZE];
    struct sockaddr_storage addr;
    socklen_t addrlen;

    printf("UDP\n");

    addrlen = sizeof(struct sockaddr_storage);
    num_recv = recvfrom(sfd, buf, BUF_SIZE, 0, (struct sockaddr *) &addr, &addrlen);
    if (num_recv == -1) {
        perror("recvfrom");
        exit(EXIT_FAILURE);
    }
    if (sendto(sfd, buf, num_recv, 0, (struct sockaddr *) &addr, addrlen) == -1) {
        perror("sendto");
        exit(EXIT_FAILURE);
    }
}


int
main(int argc __attribute__((unused)),
     char *argv[] __attribute__((unused)))
{
    int sfd_tcp, sfd_udp;
    struct sockaddr_in6 addr;
    int epfd;
    struct epoll_event ev;
    struct epoll_event evlist[2];
    int ready, j;
    struct sigaction sa;


    /* Create sockets.
     *  1) TCP socket
     *  2) UDP socket
     */

    sfd_tcp = socket(AF_INET6, SOCK_STREAM, 0);
    if (sfd_tcp == -1) {
        perror("socket-TCP");
        exit(EXIT_FAILURE);
    }
    sfd_udp = socket(AF_INET6, SOCK_DGRAM, 0);
    if (sfd_udp == -1) {
        perror("socket-UDP");
        exit(EXIT_FAILURE);
    }

    /* This code is not recommended. */
    memset(&addr, 0, sizeof(struct sockaddr_in));
    addr.sin6_family = AF_INET6;
    (void) inet_pton(AF_INET6, "::", &addr.sin6_addr);
    addr.sin6_port = htons(UDP_ECHO_SERVICE);
    if (bind(sfd_udp, (struct sockaddr *) &addr, sizeof(struct sockaddr_in6)) == -1) {
        perror("bind_UDP");
        exit(EXIT_FAILURE);
    }
    addr.sin6_port = htons(TCP_ECHO_SERVICE);
    if (bind(sfd_tcp, (struct sockaddr *) &addr, sizeof(struct sockaddr_in6)) == -1) {
        perror("bind-TCP");
        exit(EXIT_FAILURE);
    }
    if (listen(sfd_tcp, BACKLOG) == -1) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    /* Ensure zombie processes terminate. */
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    sa.sa_handler = grim_ripper;
    if (sigaction(SIGCHLD, &sa, NULL) == -1) {
        perror("sigaction");
        exit(EXIT_FAILURE);
    }

    /* Watch sockets using epoll API */
    epfd = epoll_create(1);
    if (epfd == -1) {
        perror("epoll_create");
        exit(EXIT_FAILURE);
    }
    ev.events = EPOLLIN;
    ev.data.fd = sfd_tcp;
    if (epoll_ctl(epfd, EPOLL_CTL_ADD, sfd_tcp, &ev) == -1) {
        perror("epoll_ctl-EPOLL_CTL_ADD");
        exit(EXIT_FAILURE);
    }
    ev.events = EPOLLIN;
    ev.data.fd = sfd_udp;
    if (epoll_ctl(epfd, EPOLL_CTL_ADD, sfd_udp, &ev) == -1) {
        perror("epoll_ctl-EPOLL_CTL_ADD");
        exit(EXIT_FAILURE);
    }

    while (1) {
        ready = epoll_wait(epfd, evlist, 2, -1);
        if (ready == -1) {
            if (errno == EINTR) {
                continue;
            }
            perror("epoll_wait");
            exit(EXIT_FAILURE);
        }
        for (j = 0; j < ready; ++j) {
            if (evlist[j].events & EPOLLIN) {
                printf("I/O is available; fd=%d\n", evlist[j].data.fd);
                if (evlist[j].data.fd == sfd_tcp) {
                    (void) serve_tcpecho(sfd_tcp);
                } else if (evlist[j].data.fd == sfd_udp) {
                    (void) serve_udpecho(sfd_udp);
                } else {
                    assert(0);
                }
            }
            if (evlist[j].events & (EPOLLHUP | EPOLLERR)) {
                if (epoll_ctl(epfd, EPOLL_CTL_DEL, evlist[j].data.fd, NULL) == -1) {
                    perror("epoll_ctl-EPOLL_CTL_DEL");
                    exit(EXIT_FAILURE);
                }
                (void) close(evlist[j].data.fd);
            }
        }
    }

    exit(EXIT_SUCCESS);
}
