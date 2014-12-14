/*************************************************************************\
*                  Copyright (C) xirc, 2014.                              *
*                                                                         *
* This program is free software. You may use, modify, and redistribute it *
* under the terms of the GNU Affero General Public License as published   *
* by the Free Software Foundation, either version 3 or (at your option)   *
* any later version. This program is distributed without any warranty.    *
* See the file COPYING.agpl-v3 for details.                               *
\*************************************************************************/


#include <sys/types.h>
#include <sys/sem.h>
#include <errno.h>
#include <unistd.h>

#include "semun.h"
#include "event_flag.h"


int bs_use_sem_undo = 0;
int bs_retry_on_eintr = 1;


/* SET is 1, CLEAR is 0 */
const int FLAG_SET = 1;
const int FLAG_CLEAR = 0;


/* Initialize semaphore (default is clear) */
int
init_event_flag(int sem_id, int sem_num)
{
    union semun arg;

    arg.val = 1;
    return semctl(sem_id, sem_num, SETVAL, arg);
}


/* Set event flag - decrement it by 1 */
int
set_event_flag(int sem_id, int sem_num)
{
    struct sembuf sops;

    sops.sem_num = sem_num;
    sops.sem_op = -1;
    sops.sem_flg = bs_use_sem_undo ? SEM_UNDO : 0;

    while (semop(sem_id, &sops, 1) == -1) {
        if (errno != EINTR || !bs_retry_on_eintr) {
            return -1;
        }
    }
    return 0;
}


/* Clear event flag - increment it by 1 */
int
clear_event_flag(int sem_id, int sem_num)
{
    struct sembuf sops;

    sops.sem_num = sem_num;
    sops.sem_op = +1;
    sops.sem_flg = bs_use_sem_undo ? SEM_UNDO : 0;

    return semop(sem_id, &sops, 1);
}


/* Wait for event flag is set */
int
wait_for_event_flag(int sem_id, int sem_num)
{
    struct sembuf sops;
    sops.sem_num = sem_num;
    sops.sem_op = 0;
    sops.sem_flg = bs_use_sem_undo ? SEM_UNDO : 0;

    while (semop(sem_id, &sops, 1) == -1) {
        if (errno != EINTR || !bs_retry_on_eintr) {
            return -1;
        }
    }
    return 0;
}

int
get_flag_state(int sem_id, int sem_num)
{
    int val;

    val = semctl(sem_id, sem_num, GETVAL, NULL);
    if (val == -1) {
        return -1;
    }

    if (val == 0) {
        return FLAG_SET;
    }
    return FLAG_CLEAR;
}
