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


/* acct_view.c

   Display contents of a process accounting file.
*/


#define _BSD_SOURCE
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <time.h>
#include <sys/stat.h>
#include <sys/acct.h>
#include <limits.h>
#include <pwd.h>

#define TIME_BUF_SIZE 100


/* Convert comp_t value into long long */
static long long
comptToLL(comp_t ct)
{
    const int EXP_SIZE = 3;         /* 3-bit, base-8 exponent */
    const int MANTISSA_SIZE = 13;   /* Followed by 13-bit mantissa */
    const int MANTISSA_MASK = (1 << MANTISSA_SIZE) - 1;
    long long mantissa, exp;

    mantissa = ct & MANTISSA_MASK;
    exp = (ct >> MANTISSA_SIZE) & ((1 << EXP_SIZE) -1);
    return mantissa << (exp *3);    /* Power of 8 = left shift 3 bits */
}


/* Convert uid value into user name */
static char *
uname_from_uid(uid_t uid)
{
    struct passwd *pwd;

    pwd = getpwuid(uid);
    return (pwd == NULL) ? NULL : pwd->pw_name;
}


int
main(int argc, char *argv[])
{

    int acct_file;
    struct acct ac;
    ssize_t nread;
    char *s;
    char time_buf[TIME_BUF_SIZE];
    struct tm *loc;
    time_t t;

    if (argc != 2 || strcmp(argv[1], "--help") == 0) {
        fprintf(stderr, "usage: %s file\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    acct_file = open(argv[1], O_RDONLY);
    if (acct_file == -1) {
        perror("open");
        exit(EXIT_FAILURE);
    }

    printf("command  flags   term.  user     "
            "start time            CPU   elapsed\n");
    printf("                status           "
            "                      time     time\n");

    while ((nread = read(acct_file, &ac, sizeof(struct acct))) > 0) {
        if (nread != sizeof(struct acct)) {
            fprintf(stderr, "read: partial read\n");
            exit(EXIT_FAILURE);
        }

        printf("%-8.8s  ", ac.ac_comm);

        printf("%c", (ac.ac_flag & AFORK) ? 'F' : '-');
        printf("%c", (ac.ac_flag & ASU)   ? 'S' : '-');
        printf("%c", (ac.ac_flag & AXSIG) ? 'X' : '-');
        printf("%c", (ac.ac_flag & ACORE) ? 'C' : '-');

#ifdef __linux__
        printf(" %#6lx   ", (unsigned long) ac.ac_exitcode);
#else /* Many other implementations provide ac_stat instead */
        printf(" %#6lx   ", (unsigned long) ac.ac_stat);
#endif

        s = uname_from_uid(ac.ac_uid);
        printf("%-8.8s ", (s == NULL) ? "???" : s);

        t = ac.ac_btime;
        loc = localtime(&t);
        if (loc == NULL) {
            printf("???Unknown time???  ");
        } else {
            strftime(time_buf, TIME_BUF_SIZE, "%Y-%m-%d %T ", loc);
            printf("%s ", time_buf);
        }

        printf("%5.2f %7.2f ",
                (double) (comptToLL(ac.ac_utime) + comptToLL(ac.ac_stime)) /
                    sysconf(_SC_CLK_TCK),
                (double) comptToLL(ac.ac_etime) / sysconf(_SC_CLK_TCK));
        printf("\n");
    }
    if (nread == -1) {
        perror("read");
        exit(EXIT_FAILURE);
    }

    exit(EXIT_SUCCESS);
}
