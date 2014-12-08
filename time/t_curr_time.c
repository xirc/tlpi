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

#include "curr_time.h"


int
main(int argc __attribute__((unused)),
     char *argv[] __attribute__((unused)))
{
    printf("%s\n", curr_time(NULL));
    printf("%s\n", curr_time("%H:%M:%S %A %d %B %Y %Z"));
    printf("%s\n", curr_time("%F %T"));

    exit(EXIT_SUCCESS);
}
