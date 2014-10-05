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


/* dump_utmpx.c

   Display the contents of the utmp-style file named on the command line.

   This version of the program differs from that which appears in the book in
   that it prints extra information from each utmpx record.

   This program is Linux-specific.
*/


#define _GNU_SOURCE
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <utmpx.h>
#include <paths.h>


int
main(int argc, char *argv[])
{
    struct utmpx *ut;

    if (argc > 1 && strcmp(argv[1], "--help") == 0) {
        fprintf(stderr, "usage: %s [utmp-pathname]\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    /* Use alternate file if supplied */
    if (argc > 1) {
        if (utmpxname(argv[1]) == -1) {
            perror("utmpxname");
        }
    }

    setutxent();
    printf("user     type        PID line   id   host     date/time\n");
    /* Sequential scan to EOF */
    while ((ut = getutxent()) != NULL) {
        printf("%-8s ", ut->ut_user);
        printf("%-9.9s ",
                (ut->ut_type == EMPTY) ? "EMPTY" :
                (ut->ut_type == RUN_LVL) ? "RUN_LVL" :
                (ut->ut_type == BOOT_TIME) ? "BOOT_TIME" :
                (ut->ut_type == NEW_TIME) ? "NEW_TIME" :
                (ut->ut_type == OLD_TIME) ? "OLD_TIME" :
                (ut->ut_type == INIT_PROCESS) ? "INIT_PR" :
                (ut->ut_type == LOGIN_PROCESS) ? "LOGIN_PR" :
                (ut->ut_type == USER_PROCESS) ? "USER_PR" :
                (ut->ut_type == DEAD_PROCESS) ? "DEAD_PR" : "???");
        printf("%5ld %-6.6s %-3.5s %-9.9s ",
                (long) ut->ut_pid, ut->ut_line, ut->ut_id, ut->ut_host);
        printf("%s", ctime(&(ut->ut_tv.tv_sec)));
    }

    endutxent();
    exit(EXIT_SUCCESS);
}
