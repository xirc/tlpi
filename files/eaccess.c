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


static int
eaccess(const char *pathname, int mode)
{
    int uid, euid, retval;
    uid = getuid();
    euid = geteuid();

    if (setreuid(euid, uid) == -1) {
        return -1;
    }

    retval = access(pathname, mode);

    if (setreuid(uid, euid) == -1) {
        return -1;
    }

    return retval;
}


int
main(int argc, char *argv[])
{
    int f_ok, r_ok, w_ok, x_ok;
    char *pathname;

    if (argc != 2 || strcmp(argv[1], "--help") == 0) {
        fprintf(stderr, "Usage: %s filepath\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    pathname = argv[1];

    printf("uid=%ld euid=%ld\n", (long)getuid(), (long)geteuid());

    f_ok = !access(pathname, F_OK);
    r_ok = !access(pathname, R_OK);
    w_ok = !access(pathname, W_OK);
    x_ok = !access(pathname, X_OK);
    printf("access: FRWX (%1d%1d%1d%1d)\n", f_ok, r_ok, w_ok, x_ok);

    f_ok = !eaccess(pathname, F_OK);
    r_ok = !eaccess(pathname, R_OK);
    w_ok = !eaccess(pathname, W_OK);
    x_ok = !eaccess(pathname, X_OK);
    printf("eaccess: FRWX (%1d%1d%1d%1d)\n", f_ok, r_ok, w_ok, x_ok);

    exit(EXIT_SUCCESS);
}
