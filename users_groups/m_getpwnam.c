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
#include <pwd.h>
#include <string.h>


static struct passwd *
m_getpwnam(const char *name)
{
    struct passwd *pw;
    int _errno = errno;

    setpwent();
    while (errno = 0,
           (pw = getpwent()) != NULL)
    {
        if (strcmp(pw->pw_name, name) == 0) {
            // found the entry!
            errno = _errno;
            return pw;
        }
    }

    endpwent();
    return NULL;
}


int
main(int argc, char *argv[])
{
    struct passwd *pw;

    if (argc != 2 || strcmp(argv[1], "--help") == 0) {
        fprintf(stderr, "usage: %s uname\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    errno = 0;
    pw = m_getpwnam(argv[1]);
    if (pw == NULL) {
        if (errno == 0) {
            fprintf(stderr, "cannot found uname=%s\n", argv[1]);
        } else {
            perror("m_getpwnam");
        }
        exit(EXIT_FAILURE);
    }

    printf("pw_name: %s\n", pw->pw_name);
    printf("pw_passwd: %s\n", pw->pw_passwd);
    printf("pw_uid: %d\n", pw->pw_uid);
    printf("pw_gid: %d\n", pw->pw_gid);
    printf("pw_gecos: %s\n", pw->pw_gecos);
    printf("pw_dir: %s\n", pw->pw_dir);
    printf("pw_shell: %s\n", pw->pw_shell);

    exit(EXIT_SUCCESS);
}
