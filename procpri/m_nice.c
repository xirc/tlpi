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
#include <string.h>
#include <sys/resource.h>
#include <getopt.h>
#include <limits.h>
#include <ctype.h>


#define PROGNAME "m_nice"
#define VERSION "0.1"
#define DEFAULT_ADJUST_NICE 10


#define TOL_EXACT 0x01
static int
to_long(char const *str, long *val, int flags)
{
    char *endptr;
    long rc;

    if (str == NULL || val == NULL) {
        errno = EINVAL;
        return -1;
    }

    rc = strtol(str, &endptr, 0);
    if (errno == ERANGE && (rc == LONG_MAX || rc == LONG_MIN)) {
        /* RANGE ERROR */
        return -1;
    }
    if (errno != 0 && rc == 0) {
        /* OTHER ERROR */
        return -1;
    }

    if (rc == 0 && str == endptr) {
        /* Empty string */
        errno = EINVAL;
        return -1;
    }

    if (*endptr != '\0' && (flags & TOL_EXACT)) {
        /* Further characters after digits */
        errno = EINVAL;
        return -1;
    }

    *val = rc;
    return 0;
}


static int nice_value = DEFAULT_ADJUST_NICE;
static struct option long_options[] =
    {
        {"adjustment", optional_argument, NULL, 'n'},
        {"help", no_argument, NULL, 'h'},
        {"version", no_argument, NULL, 'v'},
        {0, 0, 0, 0}
    };


static void
usage(FILE *fp)
{
    fprintf(fp,
       "usage: %s [OPTION] [COMMAND [ARG]...]\n", PROGNAME);
    fprintf(fp,
        "  Run COMMAND with an adjusted niceness, "
        "which affects process scheduling.\n"
        "  With no COMMAND, print the current niceness.\n"
        "  Nicenesses range from "
        "-20 (most favorable scheduling) to 19 (least favorable).\n"
        "\n"
        "    [OPTION]\n");
    fprintf(fp,
        "       -n, --adjustment=N       add integer N "
        "to the niceness (default %d)\n", DEFAULT_ADJUST_NICE);
    fprintf(fp,
        "       -h, --help               show usage\n"
        "       -v, --version            show version and exit\n");
}


static void
version(FILE *fp)
{
    fprintf(fp, "%s version %s\n", PROGNAME, VERSION);
    exit(EXIT_SUCCESS);
}


int
main(int argc, char *argv[])
{
    int c, option_index;
    int j, rc;

    c = getopt_long_only(argc, argv, ":n:",
            long_options, &option_index);
    switch (c) {
    case -1: /* the end of option */
        break;
    case 'n':
        if (optarg == NULL) {
            /* no argument */
            break;
        }
        rc = to_long(optarg, (long*)&nice_value, TOL_EXACT);
        if (rc == -1) {
            fprintf(stderr, "%s: invalid adjustment %s\n", PROGNAME, optarg);
            exit(EXIT_FAILURE);
        }
        break;
    case 'h':
        usage(stdout);
        exit(EXIT_SUCCESS);
    case 'v':
        version(stdout);
        exit(EXIT_SUCCESS);
    case ':':
        /* Missing argument */
        if (strcmp(argv[optind-1], "-n") == 0) {
            /* option '-n's argument is optional */
            break;
        }
        printf("%s: option %s require argument\n", PROGNAME, argv[optind-1]);
        exit(EXIT_FAILURE);
    default:
        if (argv[optind-1][0] == '-') {
            rc = to_long(argv[optind-1]+1, (long*)&nice_value, TOL_EXACT);
            if (rc == 0) {
                /* Valid option and adjustment (e.g. -10, --4) */
                break;
            }
            rc = to_long(argv[optind-1]+1, (long*)&nice_value, 0);
            if (rc == 0) {
                /* Invalid adjustmnet */
                fprintf(stderr,
                    "%s: invalid adjustment %s\n", PROGNAME, argv[optind-1]);
                exit(EXIT_FAILURE);
            }
        }
        fprintf(stderr, "%s: invalid option %s\n", PROGNAME, argv[optind-1]);
        exit(EXIT_FAILURE);
    }

    if (nice_value < -20 || nice_value > 19) {
        fprintf(stderr,
            "%s: invalid nice value (value=%d)\n", PROGNAME, nice_value);
        exit(EXIT_FAILURE);
    }

    /* case: no argument
     * retrieve current nive value of this process */
    if (argc == 1) {
        rc = getpriority(PRIO_PROCESS, 0);
        if (rc == -1) {
            fprintf(stderr, "%s: cannot get priority\n", PROGNAME);
            exit(EXIT_FAILURE);
        }
        printf("%ld\n", (long) rc);
        exit(EXIT_SUCCESS);
    }

    /* case: exec a command with the priority */
    if (argc <= optind) {
        /* no command */
        fprintf(stderr,
            "%s: you should feed command (with argument) to me.\n", PROGNAME);
        exit(EXIT_FAILURE);
    }

    rc = setpriority(PRIO_PROCESS, 0, nice_value);
    if (rc == -1) {
        fprintf(stderr, "%s: cannot set priority\n", PROGNAME);
        exit(EXIT_FAILURE);
    }

    rc = execvp(argv[optind], argv+optind);
    if (rc == -1) {
        fprintf(stderr,
            "%s: cannot execute command '%s", PROGNAME, argv[optind]);
        for (j = optind+1; j < argc; ++j) {
            printf(" %s", argv[j]);
        }
        printf("'\n");
    }
    exit(EXIT_FAILURE);
}
