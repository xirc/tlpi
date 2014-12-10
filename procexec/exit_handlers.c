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


/* exit_handlers.c

   Demonstrate the use of atexit(3) and on_exit(3), which can be used to
   register functions (commonly known as "exit handlers") to be called at
   normal process exit. (These functions are not called if the process does
   an _exit(2) or if it is terminated by a signal.)
*/


#define _BSD_SOURCE
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>


static void
atexit_func1(void)
{
    printf("atexit function 1 called\n");
}


static void
atexit_func2(void)
{
    printf("atexit function 2 called\n");
}


static void
on_exit_func(int exit_status, void *arg)
{
    printf("on_exit function called: status=%d arg=%ld\n",
            exit_status, (long) arg);
}


int
main(int argc __attribute__((unused)),
     char *argv[] __attribute__((unused)))
{
    if (on_exit(on_exit_func, (void*) 10) != 0) {
        perror("on_exit 1");
        exit(EXIT_FAILURE);
    }
    if (atexit(atexit_func1) != 0) {
        perror("atexit 1");
        exit(EXIT_FAILURE);
    }
    if (atexit(atexit_func2) != 0) {
        perror("atexit 2");
        exit(EXIT_FAILURE);
    }
    if (on_exit(on_exit_func, (void*) 20) != 0) {
        perror("on_exit 2");
        exit(EXIT_FAILURE);
    }

    exit(2);
}
