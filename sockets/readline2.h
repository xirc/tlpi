/*************************************************************************\
*                  Copyright (C) xirc, 2014.                              *
*                                                                         *
* This program is free software. You may use, modify, and redistribute it *
* under the terms of the GNU Affero General Public License as published   *
* by the Free Software Foundation, either version 3 or (at your option)   *
* any later version. This program is distributed without any warranty.    *
* See the file COPYING.agpl-v3 for details.                               *
\*************************************************************************/


#ifndef READ_LINE_H
#define READ_LINE_H


#include <sys/types.h>


struct rlbuf {
    int fd;
    char *buffer;
    size_t bytes;
    size_t size;
    char *next;
};


int readline_init(struct rlbuf *rlbuf, int fd, size_t size);


int readline_free(struct rlbuf *rlbuf);


ssize_t readline(struct rlbuf *rlbuf, char *buffer, size_t n);


#endif
