/*************************************************************************\
*                  Copyright (C) xirc, 2014.                              *
*                                                                         *
* This program is free software. You may use, modify, and redistribute it *
* under the terms of the GNU Affero General Public License as published   *
* by the Free Software Foundation, either version 3 or (at your option)   *
* any later version. This program is distributed without any warranty.    *
* See the file COPYING.agpl-v3 for details.                               *
\*************************************************************************/


#define _GNU_SOURCE
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <getopt.h>
#include <limits.h>
#include <string.h>
#include <libgen.h>
#include <assert.h>
#include <pwd.h>
#include <shadow.h>


char progname[PATH_MAX];
static void
usage()
{
    printf("usage: %s [-u user] program-file [arg...]\n", progname);
}


/* Extract basename from 'path', result is stored in 'buf' */
/* Return 0 on success, Return -1 on errr. */
static int
safe_basename(char const * path, char *buf, size_t size)
{
    char *p, *b;

    p = strdup(path);
    if (p == NULL) {
        return -1;
    }

    b = basename(p);
    if (strlen(b) > size - 1) {
        free(p);
        return -1;
    }

    strncpy(buf, b, size);
    buf[size-1] = '\0';

    free(p);
    return 0;
}


int
main(int argc, char *argv[])
{
    int opt;
    char *username;
    long lnmax;

    struct passwd *pwd;
    struct spwd *spwd;
    int is_authorized;
    char *password, *encrypted, *p;

    /* Make program name */
    safe_basename(argv[0], progname, PATH_MAX);

    /* Make buffer for username */
    lnmax = sysconf(_SC_LOGIN_NAME_MAX);
    if (lnmax == -1) {
        /* make aguess */
        lnmax = 256;
    }
    username = malloc(lnmax);
    if (username == NULL) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }

    /* Default option */
    assert(lnmax >= 5);
    strncpy(username, "root", 5);

    /* Parse options */
    while ((opt = getopt(argc, argv, "hu:")) != -1) {
        switch (opt) {
        case 'h':
            usage();
            exit(EXIT_SUCCESS);
        case 'u':
            if (strlen(optarg) > (unsigned long) lnmax - 1) {
                fprintf(stderr, "too long username\n");
                exit(EXIT_FAILURE);
            }
            strncpy(username, optarg, lnmax);
            username[lnmax-1] = '\0';
            break;
        default:
            break;
        }
    }

    /* Program name is not found */
    if (optind >= argc) {
        usage();
        exit(EXIT_FAILURE);
    }


    /* Authorize 'user' */
    pwd = getpwnam(username);
    if (pwd == NULL) {
        perror("counldn't get password record");
        exit(EXIT_FAILURE);
    }
    spwd = getspnam(username);
    if (spwd == NULL && errno == EACCES) {
        perror("no permission to read shadow password file");
        exit(EXIT_FAILURE);
    }
    if (spwd != NULL) {
        pwd->pw_passwd = spwd->sp_pwdp;
    }
    /* Encrypt password */
    password = getpass("Password: ");
    encrypted = crypt(password, pwd->pw_passwd);
    for (p = password; *p != '\0'; ++p) {
        *p = '\0';
    }
    if (encrypted == NULL) {
        perror("crypt");
    }

    is_authorized = strcmp(encrypted, pwd->pw_passwd) == 0;
    if (!is_authorized) {
        printf("Incorrect password\n");
        exit(EXIT_FAILURE);
    }

    /* Change UID and GID of process */
    setresgid(pwd->pw_gid, pwd->pw_gid, pwd->pw_gid);
    setresuid(pwd->pw_uid, pwd->pw_uid, pwd->pw_uid);

    execvp(argv[optind], argv+optind);

    /* If we comes here, exec is failed */
    exit(EXIT_FAILURE);
}

