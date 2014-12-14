/*************************************************************************\
*                  Copyright (C) xirc, 2014.                              *
*                                                                         *
* This program is free software. You may use, modify, and redistribute it *
* under the terms of the GNU Affero General Public License as published   *
* by the Free Software Foundation, either version 3 or (at your option)   *
* any later version. This program is distributed without any warranty.    *
* See the file COPYING.agpl-v3 for details.                               *
\*************************************************************************/


#ifndef BINARY_SEMS_FIFO_H
#define BINARY_SEMS_FIFO_H


/* Use SEM_UNDO during semop() */
extern int bs_use_sem_undo;
/* Retry if semop() interrupted by signal handler ? */
extern int bs_retry_on_eintr;


struct fifosem {
    int rfd;
    int wfd;
};


int init_fifosem(char const *fifo, struct fifosem *fs);


int init_sem_available(struct fifosem *fs);


int init_sem_in_use(struct fifosem *fs);


int reserve_sem(struct fifosem *fs);


int reserve_sem_nb(struct fifosem *fs);


int release_sem(struct fifosem *fs);


#endif
