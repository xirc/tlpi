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
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <ctype.h>


#define BUFSIZE 1024
static int fd_ptoc[2];
static int fd_ctop[2];


static void
parent_process(void)
{
    char buf[BUFSIZE];
    size_t num_read;

    /* Read end of ptoc is unused */
    if (close(fd_ptoc[0]) == -1) {
        perror("PARENT: close 1");
        exit(EXIT_FAILURE);
    }
    /* Write end of ctop is unused */
    if (close(fd_ctop[1]) == -1) {
        perror("PARENT: close 2");
        exit(EXIT_FAILURE);
    }

    while (1) {
        /* Read input from stdin */
        if (fgets(buf, BUFSIZE, stdin) == NULL) {
            /* Parent ends if EOF is given */
            break;
        }
        num_read = strlen(buf);
        if (buf[num_read - 1] == '\n') {
            buf[num_read - 1] = '\0';
        }

        /* send text to child process */
        if (write(fd_ptoc[1], buf, BUFSIZE) != BUFSIZE) {
            fprintf(stderr, "PARENT: cannot write whole buffer\n");
            exit(EXIT_FAILURE);
        }

        /* receive text from child process */
        if (read(fd_ctop[0], buf, BUFSIZE) != BUFSIZE) {
            fprintf(stderr, "PARENT: cannot read whole buffer\n");
            exit(EXIT_FAILURE);
        }

        /* Echo text received from child process */
        printf("%s\n", buf);
    }

    /* Close pipe */
    if (close(fd_ptoc[1]) == -1) {
        perror("PARENT: close fd_ptoc[1]");
    }
    if (close(fd_ctop[0]) == -1) {
        perror("PARENT: close fd_ctop[0]");
    }

    exit(EXIT_SUCCESS);
}


static void
child_process(void)
{
    int j;
    char buf[BUFSIZE];
    size_t num_read;

    /* Write end of ptoc is unused */
    if (close(fd_ptoc[1]) == -1) {
        perror("CHILD: close 1");
        exit(EXIT_FAILURE);
    }
    /* Read end of ctop is unused */
    if (close(fd_ctop[0]) == -1) {
        perror("CHILD: close 2");
        exit(EXIT_FAILURE);
    }

    while (1) {
        /* receive text from parent */
        num_read = read(fd_ptoc[0], buf, BUFSIZE);
        if (num_read == 0) {
            /* Child ends if EOF is given */
            break;
        } else if (num_read != BUFSIZE) {
            fprintf(stderr, "CHILD: cannot read whole buffer\n");
            exit(EXIT_FAILURE);
        }

        /* Convert lower case -> upper case */
        for (j = 0; j < BUFSIZE; ++j) {
            buf[j] = toupper(buf[j]);
        }

        /* send text to parent process */
        if (write(fd_ctop[1], buf, BUFSIZE) != BUFSIZE) {
            fprintf(stderr, "CHILD: cannot write whole buffer\n");
            exit(EXIT_FAILURE);
        }
    }

    /* Close FILE* and pipe fd */
    if (close(fd_ptoc[0]) == -1) {
        perror("CHILD: close fd_ptoc[1]");
    }
    if (close(fd_ctop[1]) == -1) {
        perror("CHILD: close fd_ctop[0]");
    }

    exit(EXIT_SUCCESS);
}


int
main(int argc __attribute__((unused)),
     char *argv[] __attribute__((unused)))
{
    /* Open 2 pipes for communication between parent and child */
    if (pipe(fd_ptoc) == -1) {
        perror("pipe-1");
        exit(EXIT_FAILURE);
    }
    if (pipe(fd_ctop) == -1) {
        perror("pipe-2");
        exit(EXIT_FAILURE);
    }

    switch (fork()) {
    case -1:
        perror("fork");
        exit(EXIT_FAILURE);

    case 0: /* Child process*/
        child_process();
        break;

    default: /* Parent process */
        parent_process();
        break;
    }

    exit(EXIT_SUCCESS);
}
