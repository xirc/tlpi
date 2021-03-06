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


/* show_time.c

   A short program that allows us to see the effects of locale and timezone
   on some of the functions that deal with time.

   Try running this program with command lines such as the following:

        ./show_time
        TZ=":Pacific/Auckland" ./show_time
        TZ=":US/Central" ./show_time
        TZ=":CET" ./show_time
*/


#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <locale.h>

#define BUF_SIZE 200


int
main(int argc __attribute__((unused)),
     char *argv[] __attribute__((unused)))
{
    time_t t;
    struct tm *loc;
    char buf[BUF_SIZE];

    if (setlocale(LC_ALL, "") == NULL) {
        /* Use locale settings in conversions */
        perror("setlocale");
        exit(EXIT_FAILURE);
    }

    t = time(NULL);

    printf("ctime() of time() value is:  %s", ctime(&t));

    loc = localtime(&t);
    if (loc == NULL) {
        perror("localtime");
        exit(EXIT_FAILURE);
    }

    printf("asctime() of local time is:  %s", asctime(loc));

    if (strftime(buf, BUF_SIZE, "%A, %d %B %Y, %H:%M:%S %Z", loc) == 0) {
        fprintf(stderr, "strftime returned 0\n");
    }
    printf("strftime() of local time is: %s\n", buf);

    exit(EXIT_SUCCESS);
}
