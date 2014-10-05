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


#define _GNU_SOURCE
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <utmpx.h>
#include <pwd.h>
#include <lastlog.h>
#include <fcntl.h>
#include <paths.h>  /* Definition of _PATH_UTMP and _PATH_WTMP */


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


static int
write_lastlog(uid_t uid, struct lastlog const *lastlog)
{
    int llfd;

    llfd = open(_PATH_LASTLOG, O_RDWR);
    if (llfd == -1) {
        return -1;
    }

    if (lseek(llfd, uid * sizeof(struct lastlog), SEEK_SET) == -1) {
        close(llfd);
        return -1;
    }
    if (write(llfd, lastlog, sizeof(struct lastlog)) <= 0) {
        close(llfd);
        return -1;
    }

    close(llfd);
    return 0;
}


int
main(int argc, char *argv[])
{
    uid_t uid;
    struct utmpx ut;
    char *dev_name;
    int sleep_time;
    struct lastlog llog;

    if (argc < 2 || strcmp(argv[1], "--help") == 0) {
        fprintf(stderr, "usage: %s username [sleep-time]\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    /* Get uid for username */
    uid = uid_from_name(argv[1]);
    if ((int)uid < 0) {
        fprintf(stderr, "username '%s' does not exist\n", argv[1]);
        exit(EXIT_FAILURE);
    }

    /* Initialize login record for utmp and wtmp files */
    memset(&ut, 0, sizeof(struct utmpx));
    ut.ut_type = USER_PROCESS;      /* This is a user login */
    strncpy(ut.ut_user, argv[1], sizeof(ut.ut_user));
    /* Stamp with current time */
    if (time(&ut.ut_tv.tv_sec) == -1) {
        perror("time");
        exit(EXIT_FAILURE);
    }
    ut.ut_pid = getpid();

    /* Set ut_line and ut_id based on the terminal associated with 'stdin'.
     * This code assumes terminals named "/dev/[pt]t[sy]*".
     * The "/dev/" dirname is 5 characters; the "[pt]t[sy]" filename
     * prefix is 3 characters (making 8characters in all).
     */
    dev_name = ttyname(STDIN_FILENO);
    if (dev_name == NULL) {
        perror("ttyname");
        exit(EXIT_FAILURE);
    }
    if (strlen(dev_name) <= 8) {
        /* Should never happen */
        fprintf(stderr, "Terminal name is too short: %s", dev_name);
        exit(EXIT_FAILURE);
    }

    strncpy(ut.ut_line, dev_name + 5, sizeof(ut.ut_line));
    strncpy(ut.ut_id, dev_name + 8, sizeof(ut.ut_id));

    /* Initialize login entries in lastlog */
    llog.ll_time = ut.ut_tv.tv_sec;
    strncpy(llog.ll_line, ut.ut_line, sizeof(llog.ll_line));
    strncpy(llog.ll_host, "", sizeof(llog.ll_host)); /* Empty hostname */

    printf("Creating login entries in utmp, wtmp, and lastlog\n");
    printf("        using pid %ld, line %.*s, id %.*s\n",
            (long) ut.ut_pid, (int) sizeof(ut.ut_line), ut.ut_line,
            (int) sizeof(ut.ut_id), ut.ut_id);

    /* Rewind to start of utmp file */
    setutxent();
    /* Write login record to utmp */
    if (pututxline(&ut) == NULL) {
        perror("pututxline");
        exit(EXIT_FAILURE);
    }
    /* Append login record to wtmp */
    updwtmpx(_PATH_WTMP, &ut);
    /* Write record to lastlog */
    if (write_lastlog(uid, &llog) == -1) {
        perror("write_lastlog");
        exit(EXIT_FAILURE);
    }

    /* Sleep a while, so we can examine utmp and wtmp file */
    sleep_time = argc > 2 ? atoi(argv[2]) : 15;
    if (sleep_time <= 0) {
        fprintf(stderr, "sleep-time %s > 0\n", argv[2]);
        exit(EXIT_FAILURE);
    }
    sleep(sleep_time);

    /* Now do a "logout"; use values from previously initialized 'ut'.
     * except for changes below */
    ut.ut_type = DEAD_PROCESS;
    time(&ut.ut_tv.tv_sec);
        /* Stamp with logout time */
    memset(&ut.ut_user, 0, sizeof(ut.ut_user));
        /* Logout record has NULL username */
    printf("Creating logout entries in utmp and wtmp\n");
    setutxent();        /* Rewind to start of utmp file */
    /* Overwrite previous utmp record */
    if (pututxline(&ut) == NULL) {
        perror("pututxline");
        exit(EXIT_FAILURE);
    }
    updwtmpx(_PATH_WTMP, &ut);
    endutxent();

    exit(EXIT_SUCCESS);
}
