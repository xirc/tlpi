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


/* scm_cred_send.c

   Used in conjunction with scm_cred_recv.c to demonstrate passing of
   process credentials via a UNIX domain socket.

   This program sends credentials to a UNIX domain socket.

   Usage is as shown in the usageErr() call below.

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
#include "scm_cred.h"
#include "unix_sockets.h"


static void
usage(FILE *fp, const char *progname)
{
    fprintf(fp, "%s [-d] [-n] [data [PID [UID [GID]]]]\n"
                    "        -d    use datagram socket\n"
                    "        -n    don't construct explicit "
                                  "credentials structure\n", progname);
}


int
main(int argc, char *argv[])
{
    struct msghdr msgh;
    struct iovec iov;
    int data, sfd, opt;
    ssize_t ns;
    int use_datagram_socket, no_explicit;
    union {
        struct cmsghdr cmh;
        char   control[CMSG_SPACE(sizeof(struct ucred))];
                        /* Space large enough to hold a ucred structure */
    } control_un;
    struct cmsghdr *cmhp;

    /* Parse command-line arguments */

    use_datagram_socket = 0;
    no_explicit = 0;

    while ((opt = getopt(argc, argv, "dn")) != -1) {
        switch (opt) {
        case 'd':
            use_datagram_socket = 1;
            break;
        case 'n':
            no_explicit = 1;
            break;
        default:
            usage(stderr, argv[0]);
            exit(EXIT_FAILURE);
        }
    }

    /* On Linux, we must transmit at least 1 byte of real data in
       order to send ancillary data */

    msgh.msg_iov = &iov;
    msgh.msg_iovlen = 1;
    iov.iov_base = &data;
    iov.iov_len = sizeof(int);

    /* Data is optionally taken from command line */

    data = (argc > optind) ? atoi(argv[optind]) : 12345;

    /* Don't need to specify destination address, because we use
       connect() below */

    msgh.msg_name = NULL;
    msgh.msg_namelen = 0;

    if (no_explicit) {

        /* Don't construct an explicit credentials structure. (It
           is not necessary to do so, if we just want the receiver to
           receive our real credentials.) */

        printf("Not explicitly sending a credentials structure\n");
        msgh.msg_control = NULL;
        msgh.msg_controllen = 0;

    } else {
        struct ucred *ucp;

        /* Set 'msgh' fields to describe 'control_un' */

        msgh.msg_control = control_un.control;
        msgh.msg_controllen = sizeof(control_un.control);

        /* Set message header to describe ancillary data that we want to send */

        cmhp = CMSG_FIRSTHDR(&msgh);
        cmhp->cmsg_len = CMSG_LEN(sizeof(struct ucred));
        cmhp->cmsg_level = SOL_SOCKET;
        cmhp->cmsg_type = SCM_CREDENTIALS;
        ucp = (struct ucred *) CMSG_DATA(cmhp);

        /*
        We could rewrite the preceding as:

        control_un.cmh.cmsg_len = CMSG_LEN(sizeof(struct ucred));
        control_un.cmh.cmsg_level = SOL_SOCKET;
        control_un.cmh.cmsg_type = SCM_CREDENTIALS;
        ucp = (struct ucred *) CMSG_DATA(CMSG_FIRSTHDR(&msgh));
        */

        /* Use sender's own PID, real UID, and real GID, unless
           alternate values were supplied on the command line */

        ucp->pid = (argc > optind + 1 && strcmp(argv[optind + 1], "-") != 0) ?
                        strtol(argv[optind + 1], NULL, 0) : getpid();
        ucp->uid = (argc > optind + 2 && strcmp(argv[optind + 2], "-") != 0) ?
                        strtoul(argv[optind + 2], NULL, 0) : getuid();
        ucp->gid = (argc > optind + 3 && strcmp(argv[optind + 3], "-") != 0) ?
                        strtoul(argv[optind + 3], NULL, 0) : getgid();

        printf("Send credentials pid=%ld, uid=%ld, gid=%ld\n",
                (long) ucp->pid, (long) ucp->uid, (long) ucp->gid);
    }

    sfd = unix_connect(SOCK_PATH, use_datagram_socket ? SOCK_DGRAM : SOCK_STREAM);
    if (sfd == -1) {
        perror("unix_connect");
        exit(EXIT_FAILURE);
    }

    ns = sendmsg(sfd, &msgh, 0);
    if (ns == -1) {
        perror("sendmsg");
        exit(EXIT_FAILURE);
    }
    printf("sendmsg() returned %ld\n", (long) ns);

    exit(EXIT_SUCCESS);
}
