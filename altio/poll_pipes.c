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


/* poll_pipes.c

   Example of the use of poll() to monitor multiple file descriptors.

   Usage: poll_pipes num-pipes [num-writes]
                                  def = 1

   Create 'num-pipes' pipes, and perform 'num-writes' writes to
   randomly selected pipes. Then use poll() to inspect the read ends
   of the pipes to see which pipes are readable.
*/


#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <poll.h>


int
main(int argc, char *argv[])
{
    int num_pipes, j, ready, rand_pipe, num_writes;
    int (*pfds)[2];     /* File descriptors for all pipes */
    struct pollfd *pollfd;

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
    pollfd = calloc(num_pipes, sizeof(struct pollfd));
    if (pollfd == NULL) {
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

    /* Build the file desciprtor list to be supplied to poll(). This list is set
     * to contain the file descriptors for the read ends of all of the pipes. */
    for (j = 0; j < num_pipes; ++j) {
        pollfd[j].fd = pfds[j][0];
        pollfd[j].events = POLLIN;
    }

    ready = poll(pollfd, num_pipes, -1);        /* Nonblocking */
    if (ready == -1) {
        perror("poll");
        exit(EXIT_FAILURE);
    }
    printf("poll() returned: %d\n", ready);

    /* Check which pipes have data available for reading */
    for (j = 0; j < num_pipes; ++j) {
        if (pollfd[j].revents & POLLIN) {
            printf("Readable: %d %3d\n", j, pollfd[j].fd);
        }
    }

    exit(EXIT_SUCCESS);
}
