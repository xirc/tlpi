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
#include <utmpx.h>
#include <limits.h>


static char* m_getlogin(void);


int
main(int argc __attribute__((unused)),
     char *argv[] __attribute__((unused)))
{
    char *login_name;

    login_name = getlogin();
    if (login_name == NULL) {
        perror("getlogin");
        exit(EXIT_FAILURE);
    }
    printf("  getlogin: %s\n", login_name);

    login_name = m_getlogin();
    if (login_name == NULL) {
        perror("m_getlogin");
        exit(EXIT_FAILURE);
    }
    printf("m_getlogin: %s\n", login_name);

    exit(EXIT_SUCCESS);
}

static char*
m_getlogin(void)
{
    char ttyn[PATH_MAX];
    struct utmpx *ut;
    static char *loginname = NULL;
    int rc;

    /* Get tty name associated with STDIN */
    rc = ttyname_r(STDIN_FILENO, ttyn, PATH_MAX);
    if (rc > 0) {
        errno = rc;
        return NULL;
    }

    setutxent();
    /* Search Login name associated with tty name */
    /* ttyn = /dev/[line], drop first 4 characters */
    while ((ut = getutxent()) != NULL) {
        if (strcmp(ut->ut_line, ttyn+5) != 0) {
            continue;
        }
        /* Login name is found */
        if (loginname != NULL) {
            free(loginname);
        }
        loginname = strdup(ut->ut_user);
        if (loginname == NULL) {
            endutxent();
            errno = ENOMEM;
            return NULL;
        }
        endutxent();
        errno = 0;
        return loginname;
    }
    /* Login name is not found */
    endutxent();
    errno = ENOENT;
    return NULL;
}
