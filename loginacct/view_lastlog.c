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


/* view_lastlog.c

   Display lastlogin entries for users named on command line.

   This program is Linux-specific.
*/


#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <lastlog.h>
#include <paths.h>
#include <fcntl.h>
#include <pwd.h>


static uid_t
uid_from_name(char const *uname)
{
    struct passwd *pwd;
    char *endptr;
    uid_t uid;

    /* On NULL or empty string */
    if (uname == NULL || *uname == '\0') {
        return -1;
    }

    uid = strtol(uname, &endptr, 0);
    if (*endptr == '\0') {
        return uid;
    }

    pwd = getpwnam(uname);
    if (pwd == NULL) {
        return -1;
    }

    return pwd->pw_uid;
}


int
main(int argc, char *argv[])
{
    struct lastlog llog;
    int fd, j;
    uid_t uid;

    if (argc > 1 && strcmp(argv[1], "--help") == 0) {
        fprintf(stderr, "usage: %s [username...]\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    fd = open(_PATH_LASTLOG, O_RDONLY);
    if (fd == -1) {
        perror("open");
        exit(EXIT_FAILURE);
    }

    for (j = 1; j < argc; ++j) {
        uid = uid_from_name(argv[j]);
        if ((int)uid == -1) {
            printf("No such user: %s\n", argv[j]);
            continue;
        }

        if (lseek(fd, uid * sizeof(struct lastlog), SEEK_SET) == -1) {
            perror("lseek");
            exit(EXIT_FAILURE);
        }

        if (read(fd, &llog, sizeof(struct lastlog)) <= 0) {
            printf("read failed for %s\n", argv[j]);
            continue;
        }

        printf("%-8.8s %-6.6s %-20.20s %s", argv[j], llog.ll_line,
                llog.ll_host, ctime(&llog.ll_time));
    }

    close(fd);

    exit(EXIT_SUCCESS);
}
