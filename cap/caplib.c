/*************************************************************************\
*                  Copyright (C) xirc, 2014.                              *
*                                                                         *
* This program is free software. You may use, modify, and redistribute it *
* under the terms of the GNU Affero General Public License as published   *
* by the Free Software Foundation, either version 3 or (at your option)   *
* any later version. This program is distributed without any warranty.    *
* See the file COPYING.agpl-v3 for details.                               *
\*************************************************************************/


#define _BSD_SOURCE
#include <sys/types.h>
#include <sys/capability.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <limits.h>
#include <pwd.h>
#include <shadow.h>
#include <string.h>

#include "caplib.h"


/* Change setting of capability
 * in caller's effective capabilities */
int
modify_cap(int capability, int setting)
{
    cap_t caps;
    cap_value_t cap_list[1];

    /* Retrieve caller's current capabilities */

    caps = cap_get_proc();
    if (caps == NULL) {
        return -1;
    }

    /* Change setting of 'capability' in the effective set of 'caps'.
     * The third argument, 1,
     *     is the number of items in the array 'cap_list'. */
    cap_list[0] = capability;
    if (cap_set_flag(caps, CAP_EFFECTIVE, 1, cap_list, setting) == -1) {
        cap_free(caps);
        return -1;
    }

    /* Push modified capability sets back to kernel,
     * to change caller's capabilities */
    if (cap_set_proc(caps) == -1) {
        cap_free(caps);
        return -1;
    }

    /* Free the structure that was allocated by libcap */
    if (cap_free(caps) == -1) {
        return -1;
    }

    return 0;
}


/* Raise capability in caller's effective set */
int
raise_cap(int capability)
{
    return modify_cap(capability, CAP_SET);
}


/* An analogous drop_cap() (unneeded in this program),
 * could be defined as: modify_cap(capability, CAP_CLEAR); */
int
drop_cap(int capability)
{
    return modify_cap(capability, CAP_CLEAR);
}


/* Drop all capabilities from all sets */
int
drop_all_caps(void)
{
    cap_t empty;
    int s;

    empty = cap_init();
    if (empty == NULL) {
        return -1;
    }

    s = cap_set_proc(empty);

    if (cap_free(empty) == -1) {
        return -1;
    }

    return s;
}
