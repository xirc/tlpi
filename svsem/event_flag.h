/*************************************************************************\
*                  Copyright (C) xirc, 2014.                              *
*                                                                         *
* This program is free software. You may use, modify, and redistribute it *
* under the terms of the GNU Affero General Public License as published   *
* by the Free Software Foundation, either version 3 or (at your option)   *
* any later version. This program is distributed without any warranty.    *
* See the file COPYING.agpl-v3 for details.                               *
\*************************************************************************/


#ifndef EVENT_FLAG_H
#define EVENT_FLAG_H


/* Use SEM_UNDO during semop() */
extern int bs_use_sem_undo;
/* Retry if semop() interrupted by signal handler ? */
extern int bs_retry_on_eintr;


/* Semaphore SET */
extern const int FLAG_SET;
extern const int FLAG_CLEAR;


int init_event_flag(int sem_id, int sem_num);


int set_event_flag(int semid, int sem_num);


int clear_event_flag(int semid, int sem_num);


int wait_for_event_flag(int semid, int sem_num);


int get_flag_state(int semid, int sem_num);


#endif
