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


/* check_password.c

   Read a user name and password and check if they are valid.

   This program uses the shadow password file. Some UNIX implementations
   don't support this feature.
*/


/* Compile with -lcrypt */
#define _BSD_SOURCE
#ifndef _XOPEN_SOURCE
#define _XOPEN_SOURCE
#endif
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>
#include <pwd.h>
#include <shadow.h>


int
main(int argc __attribute__((unused)),
     char *argv[] __attribute__((unused)))
{
    char *username, *password, *encrypted, *p;
    struct passwd *pwd;
    struct spwd *spwd;
    int auth_ok;
    size_t len;
    long lnmax;

    /* Determine size of buffer required for a username, and allocate it */
    lnmax = sysconf(_SC_LOGIN_NAME_MAX);
    if (lnmax == -1) {                    /* If limit is indeterminate */
        lnmax = 256;                      /* make a guess */
    }

    username = malloc(lnmax);
    if (username == NULL) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }

    printf("Username: ");
    fflush(stdout);
    if (fgets(username, lnmax, stdin) == NULL) {
        exit(EXIT_FAILURE);             /* Exit on EOF */
    }

    len = strlen(username);
    if (username[len - 1] == '\n') {
        username[len - 1] = '\0';       /* Remove trailing '\n' */
    }

    /* Look up password and shadow password records for username */
    pwd = getpwnam(username);
    if (pwd == NULL) {
        fprintf(stderr, "couldn't get password record\n");
        exit(EXIT_FAILURE);
    }
    spwd = getspnam(username);
    if (spwd == NULL && errno == EACCES) {
        fprintf(stderr, "no permission to read shadow password file\n");
        exit(EXIT_FAILURE);
    }

    if (spwd != NULL) {           /* If there is a shadow password record */
        pwd->pw_passwd = spwd->sp_pwdp;     /* Use the shadow password */
    }

    password = getpass("Password: ");

    /* Encrypt password and erase cleartext version immediately */
    encrypted = crypt(password, pwd->pw_passwd);
    for (p = password; *p != '\0'; /* do nothing */) {
        *p++ = '\0';
    }

    if (encrypted == NULL) {
        perror("crypt");
        exit(EXIT_FAILURE);
    }

    auth_ok = strcmp(encrypted, pwd->pw_passwd) == 0;
    if (!auth_ok) {
        printf("Incorrect password\n");
        exit(EXIT_FAILURE);
    }

    printf("Successfully authenticated: UID=%ld\n", (long) pwd->pw_uid);

    /* Now do authenticated work... */

    exit(EXIT_SUCCESS);
}
