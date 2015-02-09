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


/* pty_master_open_bsd.c

   Implement our pty_master_open() function, based on BSD pseudoterminals.
   See comments below.

   Note: BSD pseudoterminals are not specified in SUSv3, and are considered
   obsolete on Linux.

   See also pty_master_open.c.
*/


#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>

#include "pty_master_open.h"

#define PTYM_PREFIX      "/dev/pty"
#define PTYS_PREFIX      "/dev/tty"
#define PTY_PREFIX_LEN   (sizeof(PTYM_PREFIX) - 1)
#define PTY_NAME_LEN     (PTY_PREFIX_LEN + sizeof("XY"))
#define X_RANGE          "pqrstuvwxyzabcde"
#define Y_RANGE          "0123456789abcdef"


int
pty_master_open(char *slave_name, size_t snlen)
{
    int mfd, n;
    char *x, *y;
    char master_name[PTY_NAME_LEN];

    if (PTY_NAME_LEN > snlen) {
        errno = EOVERFLOW;
        return -1;
    }

    memset(master_name, 0, PTY_NAME_LEN);
    strncpy(master_name, PTYM_PREFIX, PTY_PREFIX_LEN);

    for (x = X_RANGE; *x != '\0'; ++x) {
        master_name[PTY_PREFIX_LEN] = *x;

        for (y = Y_RANGE; *y != '\0'; ++y) {
            master_name[PTY_PREFIX_LEN + 1] = *y;

            mfd = open(master_name, O_RDWR);
            if (mfd == -1) {
                if (errno == ENOENT) {       /* No such file */
                    return -1;
                } else {
                    continue;
                }
            }

            /* Return slave name corresponding to master */
            n = snprintf(slave_name, snlen, "%s%c%c", PTYS_PREFIX, *x, *y);
            if (n >= (long)snlen) {
                errno = EOVERFLOW;
                return -1;
            } else if (n == -1) {
                return -1;
            }
            return mfd;
        }
    }

    /* Tried all ptys without success */
    return -1;
}
