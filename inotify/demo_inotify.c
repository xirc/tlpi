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


/* demo_inotify.c

   Demonstrate the use of the inotify API.

   Usage: demo_inotify pathname...

   The program monitors each of the files specified on the command line for all
   possible file events.

   This program is Linux-specific. The inotify API is available in Linux 2.6.13
   and later.
*/


#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/inotify.h>
#include <limits.h>


static void
display_inotify_event(struct inotify_event *i)
{
    printf("    wd =%2d; ", i->wd);
    if (i->cookie > 0) {
        printf("cookie =%4d; ", i->cookie);
    }

    printf("mask = ");
#define P(m) do { if (i->mask & m) printf(#m " "); } while(0)
    P(IN_ACCESS);
    P(IN_ATTRIB);
    P(IN_CLOSE_NOWRITE);
    P(IN_CLOSE_WRITE);
    P(IN_CREATE);
    P(IN_DELETE);
    P(IN_DELETE_SELF);
    P(IN_IGNORED);
    P(IN_ISDIR);
    P(IN_MODIFY);
    P(IN_MOVE_SELF);
    P(IN_MOVED_FROM);
    P(IN_MOVED_TO);
    P(IN_OPEN);
    P(IN_Q_OVERFLOW);
    P(IN_UNMOUNT);
#undef P
    printf("\n");

    if (i->len > 0) {
        printf("         name = %s\n", i->name);
    }
}


#define BUF_LEN (10 * (sizeof(struct inotify_event) + NAME_MAX + 1))

int
main(int argc, char *argv[])
{
    int inotify_fd, wd, j;
    char buf[BUF_LEN];
    ssize_t num_read;
    char *p;
    struct inotify_event *event;

    if (argc < 2 || strcmp(argv[1], "--help") == 0) {
        fprintf(stderr, "%s pathname... \n", argv[0]);
        exit(EXIT_FAILURE);
    }

    inotify_fd = inotify_init();
    if (inotify_fd == -1) {
        perror("inotify_init");
        exit(EXIT_FAILURE);
    }

    for (j = 1; j < argc; ++j) {
        wd = inotify_add_watch(inotify_fd, argv[j], IN_ALL_EVENTS);
        if (wd == -1) {
            perror("inotify_add_watch");
            exit(EXIT_FAILURE);
        }
        printf("Watching %s using wd %d\n", argv[j], wd);
    }

    while (1) {
        num_read = read(inotify_fd, buf, BUF_LEN);
        if (num_read == 0) {
            fprintf(stderr, "read() from inotify fd returned 0!\n");
            exit(EXIT_FAILURE);
        }

        if (num_read == -1) {
            perror("read");
            exit(EXIT_FAILURE);
        }

        printf("Read %ld bytes from inotify fd\n", (long) num_read);

        for (p = buf; p < buf + num_read; /* do nothing */) {
            event = (struct inotify_event *) p;
            display_inotify_event(event);
            p += sizeof(struct inotify_event) + event->len;
        }
    }

    exit(EXIT_SUCCESS);
}

