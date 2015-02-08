/*************************************************************************\
*                  Copyright (C) xirc, 2014.                              *
*                                                                         *
* This program is free software. You may use, modify, and redistribute it *
* under the terms of the GNU Affero General Public License as published   *
* by the Free Software Foundation, either version 3 or (at your option)   *
* any later version. This program is distributed without any warranty.    *
* See the file COPYING.agpl-v3 for details.                               *
\*************************************************************************/


/* select_pipes.c
 *
   Usage: select_pipes num-pipes [num-writes=1]

   Create 'num-pipes' pipes, and perform 'num-writes' writes to
   randomly selected pipes. Then use select() to inspect the read ends
   of the pipes to see which pipes are readable.
*/


#include <sys/types.h>
#include <sys/select.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>


int
main(int argc, char *argv[])
{
    int num_pipes, j, ready, rand_pipe, num_writes;
    int (*pfds)[2];     /* File descriptors for all pipes */
    int nfds;
    fd_set readfds;

    if (argc < 2 || strcmp(argv[1], "--help") == 0) {
        fprintf(stderr, "%s num-pipes [num-writes]\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    /* Parse command line arguments */
    num_pipes = strtoul(argv[1], NULL, 0);
    num_writes = (argc > 2) ? strtoul(argv[2], NULL, 0) : 1;

    /* Allocate the arrays that we use. The arrays are sized according
     * to the number of pipes specified on command line */
    pfds = calloc(num_pipes, sizeof(int [2]));
    if (pfds == NULL) {
        perror("calloc");
        exit(EXIT_FAILURE);
    }

    /* Create the number of pipes specified on command line */
    for (j = 0; j < num_pipes; ++j) {
        if (pipe(pfds[j]) == -1) {
            fprintf(stderr, "%s: pipe %d\n", strerror(errno), j);
            exit(EXIT_FAILURE);
        }
    }

    /* Perform specified number of writes to random pipes */
    srandom((int)time(NULL));
    for (j = 0; j < num_writes; ++j) {
        rand_pipe = random() % num_pipes;   /* This is NOT perfectly random. */
        printf("Writing to fd: %3d (read fd: %3d)\n",
                pfds[rand_pipe][1], pfds[rand_pipe][0]);
        if (write(pfds[rand_pipe][1], "a", 1) == -1) {
            fprintf(stderr, "%s: write %d\n", strerror(errno), pfds[rand_pipe][1]);
        }
    }

    nfds = 0;
    FD_ZERO(&readfds);
    for (j = 0; j < num_pipes; ++j) {
        if (pfds[j][0] >= FD_SETSIZE) {
            fprintf(stderr,
                    "file descriptor exceeds limit (%d)\n", FD_SETSIZE);
            exit(EXIT_FAILURE);
        }
        FD_SET(pfds[j][0], &readfds);
        nfds = (nfds > pfds[j][0] + 1) ? nfds : pfds[j][0] + 1;
    }

    ready = select(nfds, &readfds, NULL, NULL, /* Infinite */ NULL);
    if (ready == -1) {
        perror("select");
        exit(EXIT_FAILURE);
    }
    printf("select() returned: %d\n", ready);

    /* Check which pipes have data available for reading */
    for (j = 0; j < nfds; ++j) {
        if (FD_ISSET(j, &readfds)) {
            printf("Readable: %d\n", j);
        }
    }

    exit(EXIT_SUCCESS);
}
