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


/* read_line.c

   Implementation of read_line().
*/


#include <unistd.h>
#include <errno.h>

#include "read_line.h"


ssize_t
read_line(int fd, void *buffer, size_t n)
{
    ssize_t num_read;       /* # of bytes fetched by last read() */
    size_t total_read;      /* Total bytes read so far */
    char *buf;
    char c;

    if (n <= 0 || buffer == NULL) {
        errno = EINVAL;
        return -1;
    }

    /* No pointer arithmetic on "void*" */
    buf = buffer;
    total_read = 0;
    while (1) {
        num_read = read(fd, &c, 1);
        if (num_read == -1) {
            if (errno == EINTR) {
                /* Interrupted --> restart read() */
                continue;
            } else {
                /* Some other error */
                return -1;
            }
        } else if (num_read == 0) { /* EOF */
            if (total_read == 0) {
                /* No bytes read; return 0 */
                return 0;
            } else {
                /* Some bytes read; add '\0' */
                break;
            }
        } else {
            /* 'num_read' must be 1 if we get here */
            if (total_read < n - 1) {
                total_read++;
                *buf++ = c;
            }
            if (c == '\n') {
                break;
            }
        }
    }

    *buf = '\0';
    return total_read;
}
