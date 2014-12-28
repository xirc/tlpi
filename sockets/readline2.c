/*************************************************************************\
*                  Copyright (C) xirc, 2014.                              *
*                                                                         *
* This program is free software. You may use, modify, and redistribute it *
* under the terms of the GNU Affero General Public License as published   *
* by the Free Software Foundation, either version 3 or (at your option)   *
* any later version. This program is distributed without any warranty.    *
* See the file COPYING.agpl-v3 for details.                               *
\*************************************************************************/


#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>

#include "readline2.h"


int
readline_init(struct rlbuf *rlbuf, int fd, size_t size)
{
    if (rlbuf == NULL || fd < 0 || size == 0) {
        errno = EINVAL;
        return -1;
    }

    rlbuf->buffer = (char*) malloc(sizeof(char) * size);
    if (rlbuf->buffer == NULL) {
        errno = ENOMEM;
        return -1;
    }
    rlbuf->fd = fd;
    rlbuf->size = size;
    rlbuf->bytes = 0;
    rlbuf->next = rlbuf->buffer;

    return 0;
}


int
readline_free(struct rlbuf *rlbuf)
{
    if (rlbuf == NULL) {
        errno = EINVAL;
        return -1;
    }

    if (rlbuf->buffer != NULL) {
        free(rlbuf->buffer);
    }
    rlbuf->buffer = NULL;
    rlbuf->fd = 0;
    rlbuf->size = 0;
    rlbuf->bytes = 0;
    rlbuf->next = NULL;

    return 0;
}


static ssize_t
copy(char *dest, const char *src, size_t destlen, size_t srclen)
{
    size_t i;
    size_t maxlen;
    size_t copylen;

    if (src == NULL || dest == NULL) {
        errno = EINVAL;
        return -1;
    }
    if (srclen == 0) {
        return 0;
    }

    maxlen = destlen < srclen ? destlen : srclen;

    copylen = srclen;
    for (i = 0; i < srclen; ++i) {
        if (src[i] == '\n') {
            copylen = i + 1;
            break;
        }
    }

    (void) memcpy(dest, src, maxlen);
    return copylen;
}


static ssize_t
_readline(struct rlbuf *rlbuf, char *buffer, size_t n, int *more)
{
    ssize_t num_copied;

    if (rlbuf == NULL || buffer == NULL) {
        errno = EINVAL;
        return -1;
    }
    if (rlbuf->bytes == 0) {
        errno = EINVAL;
        return -1;
    }

    num_copied = copy(buffer, rlbuf->next, n, rlbuf->bytes);
    if (more != NULL) {
        *more = (rlbuf->next[num_copied - 1] != '\n');
    }
    rlbuf->bytes -= num_copied;
    rlbuf->next += num_copied;
    return num_copied;
}


static ssize_t
readline_sync(struct rlbuf *rlbuf)
{
    ssize_t num_read;

    if (rlbuf->bytes > 0) {
        return rlbuf->bytes;
    }

    num_read = read(rlbuf->fd, rlbuf->buffer, rlbuf->size);
    if (num_read == -1) {
        return -1;
    }
    rlbuf->next = rlbuf->buffer;
    rlbuf->bytes = num_read;
    return num_read;
}


ssize_t
readline(struct rlbuf *rlbuf, char *buffer, size_t n)
{
    ssize_t num_read;
    ssize_t num_copied;
    size_t total_copied;

    char *buf;
    size_t len;
    int more;

    if (rlbuf == NULL || buffer == NULL) {
        errno = EINVAL;
        return -1;
    }
    if (rlbuf->buffer == NULL || rlbuf->fd < 0 || rlbuf->size == 0) {
        errno = EINVAL;
        return -1;
    }

    total_copied = 0;
    buf = buffer;
    len = n;
    while (1) {
        num_read = readline_sync(rlbuf);
        if (num_read == -1) {
            return -1;
        }
        if (num_read == 0) {
            return 0;
        }

        num_copied = _readline(rlbuf, buf, len, &more);
        if (num_copied == -1) {
            return -1;
        }
        total_copied += num_copied;
        if (!more) {
            break;
        }

        if (len < (size_t) num_copied) {
            buf = buffer;
            len = 0;
        } else {
            buf += num_copied;
            len -= num_copied;
        }
    }
    if (total_copied > n - 1) {
        total_copied = n - 1;
    }
    buffer[total_copied] = '\0';

    return total_copied;
}
