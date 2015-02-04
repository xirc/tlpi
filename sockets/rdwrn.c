/*************************************************************************\
*                  Copyright (C) Michael Kerrisk, 2014.                   *
*                                                                         *
* This program is free software. You may use, modify, and redistribute it *
* under the terms of the GNU Lesser General Public License as published   *
* by the Free Software Foundation, either version 3 or (at your option)   *
* any later version. This program is distributed without any warranty.    *
* See the files COPYING.lgpl-v3 and COPYING.gpl-v3 for details.           *
\*************************************************************************/
/*************************************************************************\
*                  Copyright (C) xirc, 2014.                              *
* See above.                                                              *
\*************************************************************************/


/* rdwrn.c

   Implementations of readn() and writen().
*/


#include <sys/types.h>
#include <unistd.h>
#include <errno.h>

#include "rdwrn.h"


ssize_t
readn(int fd, void *buffer, size_t n)
{
    ssize_t num_read;       /* # of bytes fetched by last read() */
    size_t total_read;      /* Total # of bytes read so far */
    char *buf;

    buf = buffer;       /* No pointer arithmetic on "void *" */
    for (total_read = 0; total_read < n; /* do nothing */) {
        num_read = read(fd, buf, n - total_read);

        if (num_read == 0) {        /* EOF */
            return total_read;      /* May be 0 if this is first read() */
        }
        if (num_read == -1) {
            if (errno == EINTR) {
                continue;           /* Interrupted --> restart read() */
            } else {
                return -1;          /* Some other error */
            }
        }

        total_read += num_read;
        buf += num_read;
    }

    return total_read;
}


ssize_t
writen(int fd, const void *buffer, size_t n)
{
    ssize_t num_written;            /* # of bytes written by last write() */
    size_t total_written;           /* Total # of bytes written so far */
    const char *buf;

    buf = buffer;       /* No pointer arithmetic on "void *" */
    for (total_written = 0; total_written < n; /* do nothing */) {
        num_written = write(fd, buf, n - total_written);

        if (num_written <= 0) {
            if (num_written == -1 && errno == EINTR) {
                continue;       /* Interrupted --> restart write() */
            } else {
                return -1;      /* Some other error */
            }
        }
        total_written += num_written;
        buf += num_written;
    }
    return total_written;
}
