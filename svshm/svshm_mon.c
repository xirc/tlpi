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


/* svshm_mon.c

   Display information from the associated data structure for the
   System V shared memory segment identified on the command line.
*/


#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>


static void
print_shm_ds(const struct shmid_ds *ds)
{
    printf("Size:                      %ld\n", (long) ds->shm_segsz);
    printf("# of attached processes:   %ld\n", (long) ds->shm_nattch);

    printf("Mode:                      %lo",
            (unsigned long) ds->shm_perm.mode);
#ifdef SHM_DEST
    printf("%s", (ds->shm_perm.mode & SHM_DEST) ? " [DEST]" : "");
#endif
#ifdef SHM_LOCKED
    printf("%s", (ds->shm_perm.mode & SHM_LOCKED) ? " [LOCKED]" : "");
#endif
    printf("\n");

    printf("Last shmat():              %s", ctime(&ds->shm_atime));
    printf("Last shmdt():              %s", ctime(&ds->shm_dtime));
    printf("Last change:               %s", ctime(&ds->shm_ctime));

    printf("Creator PID:               %ld\n", (long) ds->shm_cpid);
    printf("PID of last attach/detach: %ld\n", (long) ds->shm_lpid);

    printf("Key:                       %d\n", ds->shm_perm.__key);
    printf("UID:                       %ld\n", (long) ds->shm_perm.uid);
    printf("GID:                       %ld\n", (long) ds->shm_perm.gid);
    printf("CUID:                      %ld\n", (long) ds->shm_perm.cuid);
    printf("CGID:                      %ld\n", (long) ds->shm_perm.cgid);
    printf("MODE:                      %o\n", ds->shm_perm.mode);
    printf("SEQ:                       %d\n", ds->shm_perm.__seq);
}


int
main(int argc, char *argv[])
{
    int shmid;
    struct shmid_ds ds;

    if (argc != 2 || strcmp(argv[1], "--help") == 0) {
        fprintf(stderr, "Usage: %s shmid\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    shmid = atoi(argv[1]);
    if (shmctl(shmid, IPC_STAT, &ds) == -1) {
        perror("shmctl");
        exit(EXIT_FAILURE);
    }
    print_shm_ds(&ds);

    exit(EXIT_SUCCESS);
}
