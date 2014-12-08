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
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <pwd.h>
#include <grp.h>
#include <limits.h>
#include <string.h>


static int
m_initgroups(const char *user, gid_t group);


static char*
grp2str(gid_t group, char* default_)
{
    struct group *gr;
    gr = getgrgid(group);
    return gr == NULL ? default_ : gr->gr_name;
}


int
main(int argc __attribute__((unused)),
     char *argv[] __attribute__((unused)))
{
    /* implement initgroups using setgroups, and pwd */
    int i, ngr;
    gid_t groups[NGROUPS_MAX + 1];

    /* init groups */
    if (m_initgroups("bin", 100) == -1) {
        perror("m_initgroups");
        exit(EXIT_FAILURE);
    }

    /* get groups id */
    ngr = getgroups(NGROUPS_MAX, groups);
    if (ngr == -1) {
        perror("getgroups");
        exit(EXIT_FAILURE);
    }
    for (i = 0; i < ngr; ++i) {
        printf("%s (%d) ", grp2str(groups[i], "???"), groups[i]);
    }
    printf("\n");

    exit(EXIT_SUCCESS);
}


static int
m_initgroups(const char *user, gid_t group)
{
    struct passwd *pw;
    struct group *gr;
    gid_t groups[NGROUPS_MAX + 1];
    size_t num_gr;

    /* from argument */
    num_gr = 0;
    groups[num_gr++] = group;

    /* the group */
    pw = getpwnam(user);
    if (pw == NULL) {
        /* not found or error */
        return -1;
    }
    groups[num_gr++] = pw->pw_gid;

    /* supplimentary groups */
    while (errno = 0,
           (gr = getgrent()) != NULL)
    {
        char **unamep;
        if (groups[0] == gr->gr_gid ||
            groups[1] == gr->gr_gid) {
            /* already append */
            continue;
        }
        for (unamep = gr->gr_mem; *unamep != NULL; ++unamep) {
            if (strcmp(user, *unamep) == 0) {
                /* user belongs to this group */
                groups[num_gr++] = gr->gr_gid;
                break;
            }
        }
    }
    if (errno != 0) {
        endgrent();
        return -1;
    }
    endgrent();

    return setgroups(num_gr, groups);
}
