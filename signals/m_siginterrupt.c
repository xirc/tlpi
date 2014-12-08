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
#include <signal.h>


#define BUFSIZE 10
#define TIMEOUT 3


static int
my_siginterrupt(int sig, int flag)
{
    struct sigaction sa;

    if (sigaction(sig, NULL, &sa) == -1) {
        return -1;
    }

    if (flag == 0) {
        sa.sa_flags |= SA_RESTART;
    } else {
        sa.sa_flags &= !SA_RESTART;
    }

    if (sigaction(sig, &sa, NULL) == -1) {
        return -1;
    }
    return 0;
}


static void
read_and_print()
{
    char buffer[BUFSIZE];
    ssize_t nbytes_read;
    int i;

    nbytes_read = read(STDOUT_FILENO, buffer, BUFSIZE);
    if (nbytes_read == -1) {
        perror("read");
        return;
    }
    for (i = 0; i < nbytes_read; ++i) {
        printf("%.2X ", buffer[i]);
    }
    printf("\n");
}


static void
handler(int sig __attribute__((unused)))
{
    /* do nothing */
}


int
main(int argc __attribute__((unused)),
     char *argv[] __attribute__((unused)))
{
    struct sigaction sa;
    sa.sa_handler = handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    if (sigaction(SIGALRM, &sa, NULL) == -1) {
        perror("sigaction");
        exit(EXIT_FAILURE);
    }

    printf("Type just %d word! timeout=%ds\n", BUFSIZE, TIMEOUT);
    (void) alarm(TIMEOUT);
    read_and_print();

    printf("Type just %d word!\n", BUFSIZE);
    if (my_siginterrupt(SIGALRM, 0) == -1) {
        perror("[my]siginterrupt");
        exit(EXIT_FAILURE);
    }
    (void) alarm(TIMEOUT);
    read_and_print();

    exit(EXIT_SUCCESS);
}
