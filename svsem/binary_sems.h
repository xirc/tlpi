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


/* binary_sems.h

   Header file for binary_sems.c.
*/


#ifndef BINARY_SEMS_H
#define BINARY_SEMS_H


/* Use SEM_UNDO during semop() */
extern int bs_use_sem_undo;
/* Retry if semop() interrupted by signal handler ? */
extern int bs_retry_on_eintr;


int init_sem_available(int sem_id, int sem_num);


int init_sem_in_use(int sem_id, int sem_num);


int reserve_sem(int sem_id, int sem_num);


int release_sem(int sem_id, int sem_num);


#endif
