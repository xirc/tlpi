/*************************************************************************\
*                  Copyright (C) xirc, 2014.                              *
*                                                                         *
* This program is free software. You may use, modify, and redistribute it *
* under the terms of the GNU Affero General Public License as published   *
* by the Free Software Foundation, either version 3 or (at your option)   *
* any later version. This program is distributed without any warranty.    *
* See the file COPYING.agpl-v3 for details.                               *
\*************************************************************************/


#ifndef PSHM_XFR_H
#define PSHM_XFR_H


#include <sys/types.h>
#include <sys/stat.h>


/* Name for shared memory segment */
#define SHM_NAME "/pshm_xfr"
/* Name for semaphore set */
#define SEM_READER_NAME "/pshm_reader"
#define SEM_WRITER_NAME "/pshm_writer"

#define SEM_PERMS (S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP)


/* Allow "cc -D" to override definition */
#ifndef BUF_SIZE
/* Size of transfer buffer */
#define BUF_SIZE 1024
#endif

struct shmseg {             /* Defines structure of shared memory segment */
    int cnt;                /* Number of bytes used in 'buf' */
    char buf[BUF_SIZE];     /* Data being transferred */
};


#endif
