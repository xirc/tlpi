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
#include <string.h>
#include <fcntl.h>
#include <termios.h>

#define PASS_MAX 128


static char *
mfgets(char *buf, int size, FILE *fp)
{
    char *p;

    if (fgets(buf, size, fp) == NULL) {
        return NULL;
    }

    /* Ensure the string terminates '\0' */
    buf[size-1] = '\0';

    /* trim '\n' */
    p = strchr(buf, '\n');
    if (p != NULL) {
        *p = '\0';
    }

    return buf;
}


static char *
m_getpass(const char *prompt)
{
    static char passwd[PASS_MAX];
    struct termios our_termios;
    struct termios saved_termios;
    struct termios *p_saved_termios;
    int ttyfd;
    FILE *ttyf;
    int saved_errno;
    char *p_passwd;

    /* ttyfd == -1 indicates ttyfd is closed. */
    ttyfd = -1;
    /* ttyf == NULL indicates ttyf is closed. */
    ttyf = NULL;
    /* p_saved_termios == NULL indicates TTY is on user settings. */
    p_saved_termios = NULL;

    /* p_passwd == NULL indicates this functions fails. */
    p_passwd = NULL;

    ttyfd = open("/dev/tty", O_RDWR);
    if (ttyfd == -1) {
        goto FAIL;
    }
    ttyf = fdopen(ttyfd, "rw");
    if (ttyf == NULL) {
        goto FAIL;
    }

    if (write(ttyfd, prompt, strlen(prompt)) == -1) {
        goto FAIL;
    }

    /* Save user tty settings, then echo off */
    p_saved_termios = &saved_termios;
    if (tcgetattr(ttyfd, p_saved_termios) == -1) {
        p_saved_termios = NULL;
        goto FAIL;
    }
    our_termios = saved_termios;
    our_termios.c_lflag &= ~ECHO;
    if (tcsetattr(ttyfd, TCSAFLUSH, &our_termios) == -1) {
        goto FAIL;
    }

    /* Read password */
    if (mfgets(passwd, PASS_MAX, ttyf) == NULL) {
        goto FAIL;
    }

    /* Restore user tty settings */
    if (tcsetattr(ttyfd, TCSAFLUSH, p_saved_termios) == -1) {
        goto FAIL;
    }

    if (write(ttyfd, "\n", 1) == -1) {
        goto FAIL;
    }

    errno = 0;
    p_passwd = passwd;
    /* go through */

FAIL:
    saved_errno = errno;
    if (ttyfd != -1) {
        (void) close(ttyfd);
    }
    if (ttyf != NULL) {
        (void) fclose(ttyf);
    }
    if (p_saved_termios != NULL) {
        (void) tcsetattr(ttyfd, TCSAFLUSH, p_saved_termios);
    }
    errno = saved_errno;
    return p_passwd;
}


int
main(int argc __attribute__((unused)),
     char *argv[] __attribute__((unused)))
{
    char *passwd1, *passwd2;

    passwd1 = getpass("getpass: ");
    if (passwd1 == NULL) {
        exit(EXIT_FAILURE);
    }
    printf("passwd1= %s\n", passwd1);

    passwd2 = m_getpass("m_getpass: ");
    if (passwd2 == NULL) {
        exit(EXIT_FAILURE);
    }
    printf("passwd2= %s\n", passwd2);

    exit(EXIT_SUCCESS);
}
