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


/* ugid_functions.c

   Implements a set of functions that
   convert user/group names to user/group IDs and vice versa.
*/


#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <pwd.h>
#include <grp.h>
#include <ctype.h>
#include "ugid_functions.h"


/* Return name corresponding to 'uid', or NULL on error */
char *
user_name_from_id(uid_t uid)
{
    struct passwd *pwd;

    pwd = getpwuid(uid);
    return (pwd == NULL) ? NULL : pwd->pw_name;
}


/* Return UID corresponding to 'name', or -1 on error */
uid_t
user_id_from_name(const char *name)
{
    struct passwd *pwd;
    uid_t u;
    char *endptr;

    if (name == NULL || *name == '\0') {  /* On NULL or empty string */
        return -1;                        /* return an error */
    }

    u = strtol(name, &endptr, 10);        /* As a convenience to caller */
    if (*endptr == '\0') {                /* allow a numeric string */
        return u;
    }

    pwd = getpwnam(name);
    if (pwd == NULL) {
        return -1;
    }

    return pwd->pw_uid;
}


/* Return name corresponding to 'gid', or NULL on error */
char *
group_name_from_id(gid_t gid)
{
    struct group *grp;

    grp = getgrgid(gid);
    return (grp == NULL) ? NULL : grp->gr_name;
}


/* Return GID corresponding to 'name', or -1 on error */
gid_t
group_id_from_name(const char *name)
{
    struct group *grp;
    gid_t g;
    char *endptr;

    if (name == NULL || *name == '\0') {  /* On NULL or empty string */
        return -1;                        /* return an error */
    }

    g = strtol(name, &endptr, 10);        /* As a convenience to caller */
    if (*endptr == '\0') {                /* allow a numeric string */
        return g;
    }

    grp = getgrnam(name);
    if (grp == NULL) {
        return -1;
    }

    return grp->gr_gid;
}
