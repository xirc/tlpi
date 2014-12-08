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


/* setjmp_vars.c

   Compiling this program with and without optimization yields different
   results, since the optimizer reorganizes code and variables in a manner
   that does not take account of the dynamic flow of control established by
   a long jump.

   Try looking at the assembler source (.s) for the unoptimized (cc -S)
   and optimized (cc -O -S) versions of this program to see the cause
   of these differences.
*/


#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <setjmp.h>


static jmp_buf env;


static void
do_jump(int nvar, int rvar, int vvar)
{
    printf("Inside do_jump(): nvar=%d rvar=%d vvar=%d\n", nvar, rvar, vvar);
    longjmp(env, 1);
}


int
main(int argc __attribute__((unused)),
     char *argv[] __attribute__((unused)))
{
    int nvar;
    register volatile int rvar; /* Allocated in register if possible */
    volatile int vvar;          /* See text */

    nvar = 111;
    rvar = 222;
    vvar = 333;

    if (setjmp(env) == 0) {     /* Code executed after setjmp() */
        nvar = 777;
        rvar = 888;
        vvar = 999;
        do_jump(nvar, rvar, vvar);

    } else {                    /* Code executed after longjmp() */
        printf("After longjmp(): nvar=%d rvar=%d vvar=%d\n",
                nvar, rvar, vvar);
    }

    exit(EXIT_SUCCESS);
}
