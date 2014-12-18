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


/*  svshm_xfr.h

   Header file used by the svshm_xfr_reader.c and svshm_xfr_writer.c programs.
*/


#ifndef SVSHM_XFR_H
#define SVSHM_XFR_H


#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>

#include "binary_sems.h"


/* Key for shared memory segment */
#define SHM_KEY 0x1234
/* Key for semaphore set */
#define SEM_KEY 0x5678


#define OBJ_PERMS (S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP)


/* Write has access to shared memory */
#define WRITE_SEM 0
/* Reader has access to shared memory */
#define READ_SEM 1


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
