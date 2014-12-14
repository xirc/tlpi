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


/* print_rlimit.c

   Print the soft and hard values of a specified resource limit.
*/


#include <sys/resource.h>
#include <stdio.h>
#include "print_rlimit.h"


/* Print 'msg' followed by limits for 'resource' */
int
print_rlimit(const char *msg, int resource)
{
    struct rlimit rlim;

    if (getrlimit(resource, &rlim) == -1) {
        return -1;
    }

    printf("%s soft=", msg);
    if (rlim.rlim_cur == RLIM_INFINITY) {
        printf("infinite");
    }
#ifdef RLIM_SAVED_CUR           /* Not defined on some implementations */
    else if (rlim.rlim_cur == RLIM_SAVED_CUR) {
        printf("unrepresentaable");
    }
#endif
    else {
        printf("%lld", (long long) rlim.rlim_cur);
    }

    printf("; hard=");
    if (rlim.rlim_max == RLIM_INFINITY) {
        printf("infinite");
    }
#ifdef RLIM_SAVED_MAX           /* Not defined on some implementations */
    else if (rlim.rlim_max == RLIM_SAVED_MAX) {
        printf("unrepresentable");
    }
#endif
    else {
        printf("%lld\n", (long long) rlim.rlim_max);
    }

    return 0;
}
