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


/* strerror_tsd.c

   An implementation of strerror() that is made thread-safe through
   the use of thread-specific data.

   See also strerror_tls.c.
*/


#define _GNU_SOURCE
    /* get '_sys_nerrr' and '_sys_errlist'
     * declarations from <stdio.h> */
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>


static pthread_once_t once = PTHREAD_ONCE_INIT;
static pthread_key_t strerror_key;


#define MAX_ERROR_LEN 256
    /* Maximum length of string in per-thread
     * buffer returned by strerror() */


/* Free thread-specific data buffer */
static void
destructor(void *buf)
{
    free(buf);
}


/* One-time key creation function */
static void
create_key(void)
{
    int s;

    /* Allocate a unique thread-specific data key and save the address
     * of the destructor for thread-specific data buffers */
    s = pthread_key_create(&strerror_key, destructor);
    if (s != 0) {
        errno = s;
        perror("pthread_key_create");
        exit(EXIT_FAILURE);
    }
}


char *
strerror(int err)
{
    int s;
    char *buf;

    /* Make first caller allocate key for thread-specific data */
    s = pthread_once(&once, create_key);
    if (s != 0) {
        errno = s;
        perror("pthread_once");
        exit(EXIT_FAILURE);
    }

    buf = pthread_getspecific(strerror_key);
    if (buf == NULL) {
        /* If first call from this thread,
         * allocate buffer for thread, and save its location */
        buf = malloc(MAX_ERROR_LEN);
        if (buf == NULL) {
            perror("malloc");
            exit(EXIT_FAILURE);
        }

        s = pthread_setspecific(strerror_key, buf);
        if (s != 0) {
            errno = s;
            perror("pthread_setspecific");
            exit(EXIT_FAILURE);
        }
    }

    if (err < 0 || err >= _sys_nerr || _sys_errlist[err] == NULL) {
        snprintf(buf, MAX_ERROR_LEN, "Unknown error %d", err);
    } else {
        strncpy(buf, _sys_errlist[err], MAX_ERROR_LEN - 1);
        buf[MAX_ERROR_LEN - 1] = '\0';  /* Ensure null termination */
    }

    return buf;
}
