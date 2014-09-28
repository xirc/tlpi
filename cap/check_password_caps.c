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


/* check_password_caps.c

   This program provides an example of the use of capabilities to create a
   program that performs a task that requires privilges, but operates without
   the full power of 'root'. The program reads a username and password and
   checks if they are valid by authenticating against the (shadow) password
   file.

   The program executable file must be installed with the CAP_DAC_READ_SEACH
   permittes capability, as follows:

        $ sudo setcap "cap_dac_read_search=p" chack_password_caps

   This program is Linux-specific.

   See also check_password.c.
*/


#define _BSD_SOURCE
#include <sys/types.h>
#include <sys/capability.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <limits.h>
#include <pwd.h>
#include <shadow.h>
#include <string.h>


/* Change setting of capability
 * in caller's effective capabilities */
static int
modify_cap(int capability, int setting)
{
    cap_t caps;
    cap_value_t cap_list[1];

    /* Retrieve caller's current capabilities */

    caps = cap_get_proc();
    if (caps == NULL) {
        return -1;
    }

    /* Change setting of 'capability' in the effective set of 'caps'.
     * The third argument, 1,
     *     is the number of items in the array 'cap_list'. */
    cap_list[0] = capability;
    if (cap_set_flag(caps, CAP_EFFECTIVE, 1, cap_list, setting) == -1) {
        cap_free(caps);
        return -1;
    }

    /* Push modified capability sets back to kernel,
     * to change caller's capabilities */
    if (cap_set_proc(caps) == -1) {
        cap_free(caps);
        return -1;
    }

    /* Free the structure that was allocated by libcap */
    if (cap_free(caps) == -1) {
        return -1;
    }

    return 0;
}


/* Raise capability in caller's effective set */
static int
raise_cap(int capability)
{
    return modify_cap(capability, CAP_SET);
}


/* An analogous drop_cap() (unneeded in this program),
 * could be defined as: modify_cap(capability, CAP_CLEAR); */

/* Drop all capabilities from all sets */
static int
drop_all_caps(void)
{
    cap_t empty;
    int s;

    empty = cap_init();
    if (empty == NULL) {
        return -1;
    }

    s = cap_set_proc(empty);

    if (cap_free(empty) == -1) {
        return -1;
    }

    return s;
}


int
main(int argc, char *argv[])
{
    char *username, *password, *encrypted, *p;
    struct passwd *pwd;
    struct spwd *spwd;
    int auth_ok;
    size_t len;
    long lnmax;

    lnmax = sysconf(_SC_LOGIN_NAME_MAX);
    if (lnmax == -1) {
        /* If limit is indeterminate, make a guess */
        lnmax = 256;
    }

    username = malloc(lnmax);
    if (username == NULL) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }

    printf("Username: ");
    fflush(stdout);
    if (fgets(username, lnmax, stdin) == NULL) {
        /* Exit on EOF */
        exit(EXIT_FAILURE);
    }

    len = strlen(username);
    if (username[len - 1] == '\n') {
        username[len - 1] = '\0';
    }

    errno = 0;
    pwd = getpwnam(username);
    if (pwd == NULL) {
        if (errno != 0) {
            perror("getpwnam");
        } else {
            fprintf(stderr, "Couldn't found user %s\n", username);
        }
        exit(EXIT_FAILURE);
    }

    /* Only raise CAP_DAC_READ_SEARCH for as long as we need it */
    if (raise_cap(CAP_DAC_READ_SEARCH) == -1) {
        fprintf(stderr, "raise_cap() failed\n");
        exit(EXIT_FAILURE);
    }

    spwd = getspnam(username);
    if (spwd == NULL && errno == EACCES) {
        fprintf(stderr, "no permission to read shadow password file\n");
        exit(EXIT_FAILURE);
    }

    /* At this point, we won't need any more capabilities,
     * so drop all capabilities from all sets */
    if (drop_all_caps() == -1) {
        fprintf(stderr, "drop_all_caps() failed\n");
        exit(EXIT_FAILURE);
    }

    /* If there is a shadow password record */
    if (spwd != NULL) {
        /* Use the shadow password */
        pwd->pw_passwd = spwd->sp_pwdp;
    }

    password = getpass("Password: ");

    /* Encrypt password and erase cleartext version immediatelly */
    encrypted = crypt(password, pwd->pw_passwd);
    for (p = password; *p != '\0'; /* nothing */) {
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
