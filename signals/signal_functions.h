/*************************************************************************\
*                  Copyright (C) Michael Kerrisk, 2014.                   *
*                                                                         *
* This program is free software. You may use, modify, and redistribute it *
* under the terms of the GNU Lesser General Public License as published   *
* by the Free Software Foundation, either version 3 or (at your option)   *
* any later version. This program is distributed without any warranty.    *
* See the files COPYING.lgpl-v3 and COPYING.gpl-v3 for details.           *
\*************************************************************************/
/*************************************************************************\
*                  Copyright (C) xirc, 2014.                              *
* See above.                                                              *
\*************************************************************************/


/* signal_functions.h

   Header file for signal_functions.c.
*/


#ifndef SIGNAL_FUNCTIONS_H
#define SIGNAL_FUNCTIONS_H

#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <signal.h>


int print_sigmask(FILE *of, const char *msg);

int print_pending_sigs(FILE *of, const char *msg);

void print_sigset(FILE *of, const char *ldr, const sigset_t *mask);


#endif
