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
#include <pwd.h>


int
main(int argc, char *argv[])
{
    uid_t uid1, uid2;
    if (argc != 3 || strcmp(argv[0], "--help") == 0) {
        fprintf(stderr, "usage: %s uid1 uid2\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    uid1 = atoi(argv[1]);
    uid2 = atoi(argv[2]);

    /* wrong code */
    printf("*** wrong ***\n");
    printf("name:\n");
    printf("%s %s\n", getpwuid(uid1)->pw_name, getpwuid(uid2)->pw_name);

    /* right code */
    {
        struct passwd *user_info;
        /* Ideally, we use sysconf(_S_LOGIN_NAME_MAX) */
        char uname1[256], uname2[256];
        long lnmax = 255;

        /* user 1 */
        errno = 0;
        user_info = getpwuid(uid1);
        if (user_info == NULL) {
            if (errno == 0) {
                fprintf(stderr, "cannot found uid=%lu\n", (unsigned long) uid1);
                exit(EXIT_FAILURE);
            } else {
                perror("getpwuid");
                exit(EXIT_FAILURE);
            }
        }
        strncpy(uname1, user_info->pw_name, lnmax);
        uname1[lnmax] = '\0';

        /* user 2 */
        errno = 0;
        user_info = getpwuid(uid2);
        if (user_info == NULL) {
            if (errno == 0) {
                fprintf(stderr, "cannot find uid=%lu\n", (unsigned long) uid2);
            } else {
                perror("getpwuid");
                exit(EXIT_FAILURE);
            }
        }
        strncpy(uname2, user_info->pw_name, lnmax);
        uname2[lnmax] = '\0';

        printf("*** right ***\n");
        printf("name:\n");
        printf("%s %s\n", uname1, uname2);
    }

    exit(EXIT_SUCCESS);
}
