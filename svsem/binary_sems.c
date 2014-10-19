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


/* binary_sems.c

   Implement a binary semaphore protocol using System V semaphores.
*/


#include <sys/types.h>
#include <sys/sem.h>
#include <errno.h>

#include "semun.h"
#include "binary_sems.h"


int bs_use_sem_undo = 0;
int bs_retry_on_eintr = 1;


/* Initialize semaphore to 1 (i.e., "available") */
int
init_sem_available(int sem_id, int sem_num)
{
    union semun arg;

    arg.val = 1;
    return semctl(sem_id, sem_num, SETVAL, arg);
}


/* Initialize semaphore to 0 (i.e., "in use") */
int
init_sem_in_use(int sem_id, int sem_num)
{
    union semun arg;

    arg.val = 0;
    return semctl(sem_id, sem_num, SETVAL, arg);
}


/* Reserve semaphore (blocking), return 0 on success, or -1 with 'errno'
 * set to EINTR if operation was interrupted by a signal handler */
/* Reserve semaphore - decrement it by 1 */
int
reserve_sem(int sem_id, int sem_num)
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


/* Reserve semaphore - decrement it by 1 */
int
reserve_sem_nb(int sem_id, int sem_num)
{
    struct sembuf sops;

    sops.sem_num = sem_num;
    sops.sem_op = -1;
    sops.sem_flg = IPC_NOWAIT | (bs_use_sem_undo ? SEM_UNDO : 0);

    if (semop(sem_id, &sops, 1) == -1) {
        return -1;
    }
    return 0;
}


/* Release semaphore - increment it by 1 */
int
release_sem(int sem_id, int sem_num)
{
    struct sembuf sops;

    sops.sem_num = sem_num;
    sops.sem_op = 1;
    sops.sem_flg = bs_use_sem_undo ? SEM_UNDO : 0;

    return semop(sem_id, &sops, 1);
}
