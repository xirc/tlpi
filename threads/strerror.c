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


/* strerror.c

   An implementation of strerror() that is not thread-safe.
*/


#define _GNU_SOURCE
    /* Get '_sys_nerr' and '_sys_errlist'
     * declarations from <stdio.h> */

#include <stdio.h>
#include <string.h>     /*get declaration of strerror() */


/* Statically allocated return buffer */
#define MAX_ERROR_LEN 256
static char buf[MAX_ERROR_LEN];


char *
strerror(int err)
{
    if (err < 0 || err >= _sys_nerr || _sys_errlist[err] == NULL) {
        snprintf(buf, MAX_ERROR_LEN, "Unknown error %d", err);
    } else {
        strncpy(buf, _sys_errlist[err], MAX_ERROR_LEN - 1);
        buf[MAX_ERROR_LEN - 1] = '\0';  /* Ensure null termination */
    }

    return buf;
}
