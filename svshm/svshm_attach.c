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


/* svshm_attach.c

   Experiment with the use of shmat() to attach previously created
   System V shared memory segments.

   Usage: svshm_attach [shmid:addr[rR]]...

        r = attach with SHM_RND flag
        R = attach with SHM_RDONLY flag
*/


#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>


static void
usage(FILE *fp , char *prog_name)
{
    fprintf(fp, "Usage: %s [shmid:address[rR]]...\n", prog_name);
    fprintf(fp, "            r=SHM_RND; R=SHM_RDONLY\n");
}


int
main(int argc, char *argv[])
{
    void *addr;
    char *ret_addr, *p;
    int j, flags, shmid;

    printf("SHMLBA = %ld (%#lx), PID = %ld\n",
            (long) SHMLBA, (unsigned long) SHMLBA, (long) getpid());

    for (j = 1; j < argc; j++) {
        shmid = strtol(argv[j], &p, 0);
        if (*p != ':') {
            usage(stderr, argv[0]);
            exit(EXIT_FAILURE);
        }

        addr = (void *) strtol(p + 1, NULL, 0);
        flags = (strchr(p + 1, 'r') != NULL) ? SHM_RND : 0;
        if (strchr(p + 1, 'R') != NULL) {
            flags |= SHM_RDONLY;
        }

        ret_addr = shmat(shmid, addr, flags);
        if (ret_addr == (void *) -1) {
            fprintf(stderr, "shmat %s: %s\n", argv[j], strerror(errno));
            exit(EXIT_FAILURE);
        }

        printf("%d: %s ==> %p\n", j, argv[j], ret_addr);
    }

    printf("Sleeping 5 seconds\n");
    sleep(5);

    exit(EXIT_SUCCESS);
}
