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


/* t_chown.c

   Demonstrate the use of the chown() system call to change the owner
   and group of a file.

   Usage: t_chown owner group [file...]

   Either or both of owner and/or group can be specified as "-" to
   leave them unchanged.
*/


#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <pwd.h>
#include <grp.h>


static uid_t
userid(const char *name)
{
    struct passwd *pwd;

    pwd = getpwnam(name);
    if (pwd == NULL) {
        return -1;
    }
    return pwd->pw_uid;
}


static gid_t
groupid(const char *name)
{
    struct group *grp;

    grp = getgrnam(name);
    if (grp == NULL) {
        return -1;
    }
    return grp->gr_gid;
}


int
main(int argc, char *argv[])
{
    uid_t uid;
    gid_t gid;
    int j;
    int err_found;

    if (argc < 3 || strcmp(argv[1], "--help") == 0) {
        fprintf(stderr,
                "%s owner group [file...]\n"
                "        owner or group can be '-', "
                "meaning leave unchanged\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    if (strcmp(argv[1], "-") == 0) {      /* "-" ==> don't change owner */
        uid = -1;
    } else {                              /* Turn user name into UID */
        uid = userid(argv[1]);
        if ((long)uid == -1) {
            fprintf(stderr, "No such user (%s)\n", argv[1]);
            exit(EXIT_FAILURE);
        }
    }

    if (strcmp(argv[2], "-") == 0) {      /* "-" ==> don't change group */
        gid = -1;
    } else {                              /* Turn group name into GID */
        gid = groupid(argv[2]);
        if ((long)gid == -1) {
            fprintf(stderr, "No group user (%s)\n", argv[2]);
            exit(EXIT_FAILURE);
        }
    }

    /* Change ownership of all files named in remaining arguments */
    err_found = 0;
    for (j = 3; j < argc; j++) {
        if (chown(argv[j], uid, gid) == -1) {
            fprintf(stderr, "chown %s: %s\n", argv[j], strerror(errno));
            err_found = 1;
        }
    }

    exit(err_found ? EXIT_FAILURE : EXIT_SUCCESS);
}
