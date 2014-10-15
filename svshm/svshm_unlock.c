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



/* svshm_unlock.c

   Unlock the System V shared memory segments identified by the
   command-line arguments.

   See also svshm_lock.c.
*/


#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>


int
main(int argc, char *argv[])
{
    int j, shmid;

    for (j = 1; j < argc; j++) {
        shmid = atoi(argv[j]);
        if (shmctl(shmid, SHM_UNLOCK, NULL) == -1) {
            perror("shmctl");
            exit(EXIT_FAILURE);
        }
    }

    exit(EXIT_SUCCESS);
}
