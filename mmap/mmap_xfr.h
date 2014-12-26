/*************************************************************************\
*                  Copyright (C) xirc, 2014.                              *
*                                                                         *
* This program is free software. You may use, modify, and redistribute it *
* under the terms of the GNU Affero General Public License as published   *
* by the Free Software Foundation, either version 3 or (at your option)   *
* any later version. This program is distributed without any warranty.    *
* See the file COPYING.agpl-v3 for details.                               *
\*************************************************************************/


#ifndef MMAP_XFR_H
#define MMAP_XFR_H


#include <sys/types.h>
#include <sys/stat.h>


/* File for shared memory mapping */
#define SHARED_MEMORY_MAPPING_FILE "/tmp/mmap_xfr_buf"


/* Key for semaphore set */
#define SEM_KEY 0x5678
#define OBJ_PERMS (S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP)


/* Write has access to shared memory mapping */
#define WRITE_SEM 0
/* Reader has access to shared memory mapping */
#define READ_SEM 1


/* Allow "cc -D" to override definition */
#ifndef BUF_SIZE
/* Size of transfer buffer */
#define BUF_SIZE 1024
#endif


struct mmapseg {            /* Defines structure of shared memory mapping */
    int cnt;                /* Number of bytes used in 'buf' */
    char buf[BUF_SIZE];     /* Data being transferred */
};


#endif
