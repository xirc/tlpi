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

#include "ugid_functions.h"


int
main(int argc, char *argv[])
{
    char *username, *groupname;
    int uid, gid;

    if (argc < 2 || strcmp(argv[1], "--help") == 0) {
        fprintf(stderr, "Usage: %s (username|groupname)\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    uid = user_id_from_name(argv[1]);
    if (uid != -1) {
        printf("uid: %d\n", uid);
    }

    username = user_name_from_id(uid);
    if (username != NULL) {
        printf("username: %s\n", username);
    }

    gid = group_id_from_name(argv[1]);
    if (gid != -1) {
        printf("gid: %d\n", gid);
    }

    groupname = group_name_from_id(gid);
    if (groupname != NULL) {
        printf("groupname: %s\n", groupname);
    }

    exit(EXIT_SUCCESS);
}
