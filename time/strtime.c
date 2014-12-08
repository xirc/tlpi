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


/* strtime.c

   Demonstrate the use of strptime() and strftime().

   Calls strptime() using the given "format" to process the "input-date+time".
   The conversion is then reversed by calling strftime() with the given
   "out-format" (or a default format if this argument is omitted).
*/


#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <locale.h>

#define SBUF_SIZE 1000


static void
usage(FILE *fp, const char *progname)
{
    fprintf(fp, "%s input-date-time in-format [out-format]\n", progname);
}


int
main(int argc, char *argv[])
{
    struct tm tm;
    char sbuf[SBUF_SIZE];
    char *ofmt;

    if (argc > 1 && strcmp(argv[1], "--help") == 0) {
        usage(stdout, argv[0]);
        exit(EXIT_SUCCESS);
    }
    if (argc < 3) {
        usage(stderr, argv[0]);
        exit(EXIT_FAILURE);
    }

    /* Use locale settings in conversions */
    if (setlocale(LC_ALL, "") == NULL) {
        perror("setlocale");
        exit(EXIT_FAILURE);
    }

    memset(&tm, 0, sizeof(struct tm));          /* Initialize 'tm' */
    if (strptime(argv[1], argv[2], &tm) == NULL) {
        perror("strptime");
        exit(EXIT_FAILURE);
    }

    tm.tm_isdst = -1;           /* Not set by strptime(); tells mktime()
                                   to determine if DST is in effect */
    printf("calendar time (seconds since Epoch): %ld\n", (long) mktime(&tm));

    ofmt = (argc > 3) ? argv[3] : "%H:%M:%S %A, %d %B %Y %Z";
    if (strftime(sbuf, SBUF_SIZE, ofmt, &tm) == 0) {
        fprintf(stderr, "strftime returned 0");
        exit(EXIT_FAILURE);
    }
    printf("strftime() yields: %s\n", sbuf);

    exit(EXIT_SUCCESS);
}
