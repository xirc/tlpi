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
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <syslog.h>
#include <limits.h>
#include <string.h>
#include <libgen.h>
#include <getopt.h>


#define MAX_MESSAGE_SIZE 8096
static char progname[PATH_MAX];


static void
usage()
{
    printf("usage: %s [-ph] [message]\n", progname);
    printf(
      "         -p [PRI]:   facility.level\n"
      "                     facility = auth, authpriv, cron, daemon, ftp,\n"
      "                                local[0-7], lpr, mail, news, syslog,\n"
      "                                user, uucp\n"
      "                     level = emerg, alert, crit, err, warning,\n"
      "                             notice, info, debug\n"
      "                     default is user.info\n");
}


int
main(int argc, char *argv[])
{
    int opt;

    char *p;
    char arg0[PATH_MAX];

    char *format;
    char *s_priority, *s_facility, *s_level;
    int facility, level;
    char buf[MAX_MESSAGE_SIZE+1];

    /* Make program name */
    strncpy(arg0, argv[0], PATH_MAX);
    arg0[PATH_MAX-1] = '\0';
    p = basename(arg0);
    strncpy(progname, p, PATH_MAX);

    /* Default */
    s_priority = "user.info";
    format = NULL;                  /* use stdin */

    /* Parse options */
    while ((opt = getopt(argc, argv, "p:h")) != -1) {
        switch (opt) {
        case 'p':
            s_priority = strdup(optarg);
            if (s_priority == NULL) {
                /* E_NOMEM */
                perror("strdup");
                exit(EXIT_FAILURE);
            }
            break;
        case 'h':
            usage();
            exit(EXIT_SUCCESS);
        default:
            /* invalid option */
            usage();
            exit(EXIT_FAILURE);
        }
    }

    /* Message */
    if (optind < argc) {
        format = argv[optind];
    }

    /* Extract facility and level from priority given by arg */
    p = strchr(s_priority, '.');
    if (p == NULL) {
        fprintf(stderr, "invalid priority '%s'\n", s_priority);
        exit(EXIT_FAILURE);
    }
    s_facility = strdup(s_priority);
    if (s_facility == NULL) {
        /* E_NOMEM */
        perror("strdup");
        exit(EXIT_FAILURE);
    }
    s_facility[p - s_priority] = '\0';
    s_level = strdup(p+1);
    if (s_level == NULL) {
        /* E_NOMEM */
        perror("strdup");
        exit(EXIT_FAILURE);
    }

#define FACILITY_WHEN(sfac,fac) \
    do { if (strcmp(s_facility, sfac) == 0) facility = fac; } while (0);
    facility = -1;
    FACILITY_WHEN("auth", LOG_AUTH);
    FACILITY_WHEN("authpriv", LOG_AUTHPRIV);
    FACILITY_WHEN("cron", LOG_CRON);
    FACILITY_WHEN("daemon", LOG_DAEMON);
    FACILITY_WHEN("ftp", LOG_FTP);
    FACILITY_WHEN("local0", LOG_LOCAL0);
    FACILITY_WHEN("local1", LOG_LOCAL1);
    FACILITY_WHEN("local2", LOG_LOCAL2);
    FACILITY_WHEN("local3", LOG_LOCAL3);
    FACILITY_WHEN("local4", LOG_LOCAL4);
    FACILITY_WHEN("local5", LOG_LOCAL5);
    FACILITY_WHEN("local6", LOG_LOCAL6);
    FACILITY_WHEN("local7", LOG_LOCAL7);
    FACILITY_WHEN("lpr", LOG_LPR);
    FACILITY_WHEN("mail", LOG_MAIL);
    FACILITY_WHEN("news", LOG_NEWS);
    FACILITY_WHEN("syslog", LOG_SYSLOG);
    FACILITY_WHEN("user", LOG_USER);
    FACILITY_WHEN("uucp", LOG_UUCP);
    if (facility == -1) {
        fprintf(stderr, "invalid priority#facility '%s'\n", s_priority);
        exit(EXIT_FAILURE);
    }
#undef FACILITY_WHEN

#define LEVEL_WHEN(slev, lev) \
    do { if (strcmp(s_level, slev) == 0) level = lev; } while (0);
    level = -1;
    LEVEL_WHEN("emerge", LOG_EMERG);
    LEVEL_WHEN("alert", LOG_ALERT);
    LEVEL_WHEN("crit", LOG_CRIT);
    LEVEL_WHEN("err", LOG_ERR);
    LEVEL_WHEN("warning", LOG_WARNING);
    LEVEL_WHEN("notice", LOG_NOTICE);
    LEVEL_WHEN("info", LOG_INFO);
    LEVEL_WHEN("debug", LOG_DEBUG);
    if (level == -1) {
        fprintf(stderr, "invalid priority#level '%s'\n", s_priority);
        exit(EXIT_FAILURE);
    }
#undef LEVEL_WHEN

    openlog(progname, LOG_ODELAY, LOG_USER);
    if (format != NULL) {
        syslog(facility | level, "%s", format);
    } else {
        while ((p = fgets(buf, MAX_MESSAGE_SIZE, stdin)) != NULL) {
            p = strchr(buf, '\n');
            if (p == NULL) {
                /* fgets may drop inputs */
                fprintf(stderr,
                        "message size is too large. (<= %d)\n",
                        MAX_MESSAGE_SIZE-1);
            } else {
                /* remove '\n' */
                *p = '\0';
            }
            syslog(facility | level, "%s", buf);
        }
    }
    closelog();

    exit(EXIT_SUCCESS);
}
