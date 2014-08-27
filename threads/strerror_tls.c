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


/* strerror_tls.c

   An implementation of strerror() that is made thread-safe through
   the use of thread-local storage.

   See also strerror_tsd.c.

   Thread-local storage requires: Linux 2.6 or later, NPTL, and
   gcc 3.3 or later.
*/


#define _GNU_SOURCE
    /* Get '_sys_nerr' and '_sys_errlist'
     * declarations from <stdio.h> */
#include <stdio.h>
#include <string.h>     /* Get declarations of strettor() */
#include <pthread.h>


#define MAX_ERROR_LEN 256
static __thread char buf[MAX_ERROR_LEN];
    /* Maximum length of string in per-thread
     * buffer returned by strettor() */


char *
strerror(int err)
{
    if (err < 0 || err > _sys_nerr || _sys_errlist[err] == NULL) {
        snprintf(buf, MAX_ERROR_LEN, "Unknown error %d", err);
    } else {
        strncpy(buf, _sys_errlist[err], MAX_ERROR_LEN - 1);
        buf[MAX_ERROR_LEN - 1] = '\0';  /* Ensure null termination */
    }

    return buf;
}
