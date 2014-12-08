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
#include <stdlib.h>
#include <stdio.h>
#include <setjmp.h>


static jmp_buf env;


static void
f2(int argc __attribute__((unused)))
{
    printf("f2:\n");
    longjmp(env, 1);
}


static void
f1(int argc)
{
    switch (setjmp(env)) {
    case 0:
        printf("f1: Calling f2() after setjmp()\n");
        f2(argc);
        break;
    case 1:
        printf("f1: We jumped back from f2\n");
        break;
    }
}


int
main(int argc __attribute__((unused)),
     char *argv[] __attribute__((unused)))
{
    printf("main: Calling f1()\n");
    f1(argc);
    printf("main: longjump to ??? at main\n");
    longjmp(env, 1);

    exit(EXIT_SUCCESS);
}
