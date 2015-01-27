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


/* scm_rights_recv.c

   Used in conjunction with scm_rights_send.c to demonstrate passing of
   file descriptors via a UNIX domain socket.

   This program receives a file descriptor sent to a UNIX domain socket.

   Usage is as shown in the usage() call below.

   File descriptors can exchanged over stream or datagram sockets. This
   program uses stream sockets by default; the "-d" command-line option
   specifies that datagram sockets should be used instead.

   This program is Linux-specific.
*/


#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "scm_rights.h"
#include "unix_sockets.h"

#define BUF_SIZE 100


static void
usage(FILE *fp, const char *progname)
{
    fprintf(fp, "%s [-d]\n"
                "       -d    use datagram socket\n", progname);
}


int
main(int argc, char *argv[])
{
    struct msghdr msgh;
    struct iovec iov;
    int data, lfd, sfd, fd, opt;
    ssize_t nr;
    int use_datagram_socket;
    union {
        struct cmsghdr cmh;
        char   control[CMSG_SPACE(sizeof(int))];
                        /* Space large enough to hold an 'int' */
    } control_un;
    struct cmsghdr *cmhp;

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
        fprintf(stderr, "remove-%s\n", SOCK_PATH);
        exit(EXIT_FAILURE);
    }

    if (use_datagram_socket) {
        fprintf(stderr, "Receiving via datagram socket\n");
        sfd = unix_bind(SOCK_PATH, SOCK_DGRAM);
        if (sfd == -1) {
            perror("unix_bind");
            exit(EXIT_FAILURE);
        }
    } else {
        fprintf(stderr, "Receiving via stream socket\n");
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

    /* Set 'control_un' to describe ancillary data that we want to receive */

    control_un.cmh.cmsg_len = CMSG_LEN(sizeof(int));
    control_un.cmh.cmsg_level = SOL_SOCKET;
    control_un.cmh.cmsg_type = SCM_RIGHTS;

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
    fprintf(stderr, "recvmsg() returned %ld\n", (long) nr);

    if (nr > 0) {
        fprintf(stderr, "Received data = %d\n", data);
    }

    /* Get the received file descriptor (which is typically a different
       file descriptor number than was used in the sending process) */

    cmhp = CMSG_FIRSTHDR(&msgh);
    if (cmhp == NULL || cmhp->cmsg_len != CMSG_LEN(sizeof(int))) {
        fprintf(stderr, "bad cmsg header / message length\n");
        exit(EXIT_FAILURE);
    }
    if (cmhp->cmsg_level != SOL_SOCKET) {
        fprintf(stderr, "cmsg_level != SOL_SOCKET\n");
        exit(EXIT_FAILURE);
    }
    if (cmhp->cmsg_type != SCM_RIGHTS) {
        fprintf(stderr, "cmsg_type != SCM_RIGHTS\n");
        exit(EXIT_FAILURE);
    }

    fd = *((int *) CMSG_DATA(cmhp));
    fprintf(stderr, "Received fd=%d\n", fd);

    /* Having obtained the file descriptor, read the file's contents and
       print them on standard output */

    while (1) {
        char buf[BUF_SIZE];
        ssize_t numRead;

        numRead = read(fd, buf, BUF_SIZE);
        if (numRead == -1) {
            perror("read");
            exit(EXIT_FAILURE);
        }
        if (numRead == 0) {
            break;
        }
        write(STDOUT_FILENO, buf, numRead);
    }

    exit(EXIT_SUCCESS);
}
