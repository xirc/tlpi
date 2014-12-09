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


/* nonreentrant.c

   Demonstrate the nonreentrant nature of some library functions, in this
   example, crypt(3).
*/


#define _GNU_SOURCE
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>


static char *str2;        /* set from argv[2] */
static int handled = 0;   /* counts number of calls to handler */


static void
handler(int sig __attribute__((unused)))
{
    crypt(str2, "xx");
    handled++;
}


int
main(int argc, char *argv[])
{
    char *cr1;
    int call_num, mismatch;
    struct sigaction sa;

    if (argc != 3) {
        fprintf(stderr, "usage: %s str1 str2\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    /* make argv[2] available to handler.
     * copy statistically allocated string to another buffer.
     */
    str2 = argv[2];
    cr1 = strdup(crypt(argv[1], "xx"));
    if (cr1 == NULL) {
        perror("strdup");
        exit(EXIT_FAILURE);
    }

    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sa.sa_handler = handler;
    if (sigaction(SIGINT, &sa, NULL) == -1) {
        perror("sigaction");
        exit(EXIT_FAILURE);
    }

    /* Repatedly call crypt() using argv[1].
     * If interrupted by a signal handler,
     * then the static storage returned by crypt() will
     * be overwritten by the results of encrypting argv[2],
     * and strcmp() will detect a mismatch with the value in 'cr1'.
     */
    for (call_num = 1, mismatch = 0; ; call_num++) {
        if (strcmp(crypt(argv[1], "xx"), cr1) != 0) {
            mismatch++;
            printf("Mismatch on call %d (mismatch=%d handled=%d)\n",
                    call_num, mismatch, handled);
        }
    }

    exit(EXIT_SUCCESS);
}
