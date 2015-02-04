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


/* scm_cred_recv.c

   Used in conjunction with scm_cred_send.c to demonstrate passing of
   process credentials via a UNIX domain socket.

   This program receives credentials sent to a UNIX domain socket.

   Usage is as shown in the usage() call below.

   Credentials can exchanged over stream or datagram sockets. This program
   uses stream sockets by default; the "-d" command-line option specifies
   that datagram sockets should be used instead.

   This program is Linux-specific.
*/


#define _GNU_SOURCE
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "scm_cred.h"
#include "unix_sockets.h"


static void
usage(FILE *fp, const char *progname)
{
    fprintf(fp, "%s [-d]\n"
                "        -d    use datagram socket\n", progname);
}


int
main(int argc, char *argv[])
{
    struct msghdr msgh;
    struct iovec iov;
    struct ucred *ucredp, ucred;
    int data, lfd, sfd, optval, opt;
    ssize_t nr;
    int use_datagram_socket;
    union {
        struct cmsghdr cmh;
        char   control[CMSG_SPACE(sizeof(struct ucred))];
                        /* Space large enough to hold a ucred structure */
    } control_un;
    struct cmsghdr *cmhp;
    socklen_t len;

    /* Parse command-line arguments */

    use_datagram_socket = 0;

    while ((opt = getopt(argc, argv, "d")) != -1) {
        switch (opt) {
        case 'd':
            use_datagram_socket = 1;
            break;
        default:
            usage(stderr, argv[0]);
            exit(EXIT_FAILURE);
        }
    }

    /* Create socket bound to well-known address */

    if (remove(SOCK_PATH) == -1 && errno != ENOENT) {
        fprintf(stderr, "%s: remove-%s\n", strerror(errno), SOCK_PATH);
        exit(EXIT_FAILURE);
    }

    if (use_datagram_socket) {
        printf("Receiving via datagram socket\n");
        sfd = unix_bind(SOCK_PATH, SOCK_DGRAM);
        if (sfd == -1) {
            perror("unix_bind");
            exit(EXIT_FAILURE);
        }
    } else {
        printf("Receiving via stream socket\n");
        lfd = unix_listen(SOCK_PATH, 5);
        if (lfd == -1) {
            perror("unix_listen");
            exit(EXIT_FAILURE);
        }

        sfd = accept(lfd, NULL, 0);
        if (sfd == -1) {
            perror("accept");
            exit(EXIT_FAILURE);
        }
    }

    /* We must set the SO_PASSCRED socket option in order to receive
       credentials */
    optval = 1;
    if (setsockopt(sfd, SOL_SOCKET, SO_PASSCRED, &optval, sizeof(optval)) == -1) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }

    /* Set 'control_un' to describe ancillary data that we want to receive */
    control_un.cmh.cmsg_len = CMSG_LEN(sizeof(struct ucred));
    control_un.cmh.cmsg_level = SOL_SOCKET;
    control_un.cmh.cmsg_type = SCM_CREDENTIALS;

    /* Set 'msgh' fields to describe 'control_un' */
    msgh.msg_control = control_un.control;
    msgh.msg_controllen = sizeof(control_un.control);

    /* Set fields of 'msgh' to point to buffer used to receive (real)
       data read by recvmsg() */
    msgh.msg_iov = &iov;
    msgh.msg_iovlen = 1;
    iov.iov_base = &data;
    iov.iov_len = sizeof(int);

    msgh.msg_name = NULL;               /* We don't need address of peer */
    msgh.msg_namelen = 0;

    /* Receive real plus ancillary data */
    nr = recvmsg(sfd, &msgh, 0);
    if (nr == -1) {
        perror("recvmsg");
        exit(EXIT_FAILURE);
    }
    printf("recvmsg() returned %ld\n", (long) nr);

    if (nr > 0) {
        printf("Received data = %d\n", data);
    }

    /* Extract credentials information from received ancillary data */
    cmhp = CMSG_FIRSTHDR(&msgh);
    if (cmhp == NULL || cmhp->cmsg_len != CMSG_LEN(sizeof(struct ucred))) {
        fprintf(stderr, "bad cmsg header / message length\n");
    }
    if (cmhp->cmsg_level != SOL_SOCKET) {
        fprintf(stderr, "cmsg_level != SOL_SOCKET\n");
    }
    if (cmhp->cmsg_type != SCM_CREDENTIALS) {
        fprintf(stderr, "cmsg_type != SCM_CREDENTIALS\n");
    }

    ucredp = (struct ucred *) CMSG_DATA(cmhp);
    printf("Received credentials pid=%ld, uid=%ld, gid=%ld\n",
            (long) ucredp->pid, (long) ucredp->uid, (long) ucredp->gid);

    /* The Linux-specific, read-only SO_PEERCRED socket option returns
       credential information about the peer, as described in socket(7) */
    len = sizeof(struct ucred);
    if (getsockopt(sfd, SOL_SOCKET, SO_PEERCRED, &ucred, &len) == -1) {
        perror("getsockopt");
        exit(EXIT_FAILURE);
    }

    printf("Credentials from SO_PEERCRED: pid=%ld, euid=%ld, egid=%ld\n",
            (long) ucred.pid, (long) ucred.uid, (long) ucred.gid);

    exit(EXIT_SUCCESS);
}
