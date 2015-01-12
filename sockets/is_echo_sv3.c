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


/* is_echo_sv.c

   An implementation of the TCP "echo" service.

   NOTE: this program must be run under a root login, in order to allow the
   "echo" port (7) to be bound. Alternatively, for test purposes, you can
   replace the SERVICE name below with a suitable unreserved port number
   (e.g., "51000"), and make a corresponding change in the client.

   See also is_echo_cl.c.
*/


#define _GNU_SOURCE
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <syslog.h>
#include <sys/wait.h>
#include <getopt.h>

#include "inet_sockets.h"


#define SERVICE "echo"      /* Name of TCP service */
#define BUF_SIZE 4096
#define BACKLOG 10


/* SIGCHLD handler to reap dead child processes */
static void
grim_ripper(int sig __attribute__((unused)))
{
    int saved_errno;

    saved_errno = errno;
    while (waitpid(-1, NULL, WNOHANG) > 0) {
        continue;
    }
    errno = saved_errno;
}


/* Handle  a client request: copy socket input back to socket */
static void
handle_request(int cfd)
{
    char buf[BUF_SIZE];
    ssize_t num_read;

    while ((num_read = read(cfd, buf, BUF_SIZE)) > 0) {
        if (write(cfd, buf, num_read) != num_read) {
            syslog(LOG_ERR, "write() failed: %s", strerror(errno));
            exit(EXIT_FAILURE);
        }
    }

    if (num_read == -1) {
        syslog(LOG_ERR, "Error from read(): %s", strerror(errno));
        exit(EXIT_FAILURE);
    }
}


int
main(int argc __attribute__((unused)),
     char *argv[] __attribute__((unused)))
{
    int lfd, cfd;       /* Listening and connected sockets */
    struct sigaction sa;
    int opt, opt_use_inetd;

    opt_use_inetd = 0;
    while ((opt = getopt(argc, argv, "i")) != -1) {
        switch (opt) {
        case 'i':
            opt_use_inetd = 1;
            break;
        default:
            exit(EXIT_FAILURE);
        }
    }

    if (opt_use_inetd) {
        handle_request(STDIN_FILENO);
        exit(EXIT_SUCCESS);
    }

    if (daemon(0, 0) == -1) {
        perror("daemon");
        exit(EXIT_FAILURE);
    }

    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    sa.sa_handler = grim_ripper;
    if (sigaction(SIGCHLD, &sa, NULL) == -1) {
        syslog(LOG_ERR, "Error from sigaction(): %s", strerror(errno));
        exit(EXIT_FAILURE);
    }

    lfd = inet_listen(SERVICE, BACKLOG, NULL);
    if (lfd == -1) {
        syslog(LOG_ERR, "Could not create server socket (%s)",
                strerror(errno));
        exit(EXIT_FAILURE);
    }

    while (1) {
        cfd = accept(lfd, NULL, NULL);  /* Wait for connection */
        if (cfd == -1) {
            syslog(LOG_ERR, "Failure in accept(): %s", strerror(errno));
            exit(EXIT_FAILURE);
        }

        /* Handle each client request in a new child process */
        switch (fork()) {
        case -1:
            syslog(LOG_ERR, "Cannot create child (%s)", strerror(errno));
            close(cfd);     /* Give up on this client */
            break;          /* May be temporary; try next client */

        case 0:     /* Child */
            close(lfd);     /* Unneeded copy of listening socket */
            handle_request(cfd);
            _exit(EXIT_SUCCESS);

        default:    /* Parent */
            close(cfd);     /* Unneeded copy of connected socket */
            break;          /* Loop to accept next connection */
        }
    }

    exit(EXIT_SUCCESS);
}
