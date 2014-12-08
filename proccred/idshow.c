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


/* idshow.c

   Display all user and group identifiers associated with a process.

   Note: This program uses Linux-specific calls and the Linux-specific
   file-system user and group IDs.
*/


#define _GNU_SOURCE
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/fsuid.h>
#include <limits.h>
#include <pwd.h>
#include <grp.h>

#define SG_SIZE (NGROUPS_MAX + 1)


static char *
user_name_from_id(uid_t uid)
{
    struct passwd *pwd;

    pwd = getpwuid(uid);
    return (pwd == NULL) ? NULL : pwd->pw_name;
}


/* Return name corresponding to 'gid', or NULL on error */
static char *
group_name_from_id(gid_t gid)
{
    struct group *grp;

    grp = getgrgid(gid);
    return (grp == NULL) ? NULL : grp->gr_name;
}


int
main(int argc __attribute__((unused)),
     char *argv[] __attribute__((unused)))
{
    uid_t ruid, euid, suid, fsuid;
    gid_t rgid, egid, sgid, fsgid;
    gid_t supp_groups[SG_SIZE];
    int num_groups, j;
    char *p;

    if (getresuid(&ruid, &euid, &suid) == -1) {
        perror("getresuid");
        exit(EXIT_FAILURE);
    }
    if (getresgid(&rgid, &egid, &sgid) == -1) {
        perror("getresgid");
        exit(EXIT_FAILURE);
    }

    /* Attempts to change the file-system IDs are always ignored
       for unprivileged processes, but even so, the following
       calls return the current file-system IDs */

    fsuid = setfsuid(0);
    fsgid = setfsgid(0);

    printf("UID: ");
    p = user_name_from_id(ruid);
    printf("real=%s (%ld); ", (p == NULL) ? "???" : p, (long) ruid);
    p = user_name_from_id(euid);
    printf("eff=%s (%ld); ", (p == NULL) ? "???" : p, (long) euid);
    p = user_name_from_id(suid);
    printf("saved=%s (%ld); ", (p == NULL) ? "???" : p, (long) suid);
    p = user_name_from_id(fsuid);
    printf("fs=%s (%ld); ", (p == NULL) ? "???" : p, (long) fsuid);
    printf("\n");

    printf("GID: ");
    p = group_name_from_id(rgid);
    printf("real=%s (%ld); ", (p == NULL) ? "???" : p, (long) rgid);
    p = group_name_from_id(egid);
    printf("eff=%s (%ld); ", (p == NULL) ? "???" : p, (long) egid);
    p = group_name_from_id(sgid);
    printf("saved=%s (%ld); ", (p == NULL) ? "???" : p, (long) sgid);
    p = group_name_from_id(fsgid);
    printf("fs=%s (%ld); ", (p == NULL) ? "???" : p, (long) fsgid);
    printf("\n");

    num_groups = getgroups(SG_SIZE, supp_groups);
    if (num_groups == -1) {
        perror("getgroups");
        exit(EXIT_FAILURE);
    }

    printf("Supplementary groups (%d): ", num_groups);
    for (j = 0; j < num_groups; j++) {
        p = group_name_from_id(supp_groups[j]);
        printf("%s (%ld) ", (p == NULL) ? "???" : p, (long) supp_groups[j]);
    }
    printf("\n");

    exit(EXIT_SUCCESS);
}
