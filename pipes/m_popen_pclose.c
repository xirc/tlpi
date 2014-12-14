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
#include <sys/wait.h>
#include <limits.h>
#include <signal.h>


#define TABSIZE  8192
static pid_t pidtab[TABSIZE];
static int filenotab[TABSIZE];
static int tabsize = 0;


static void
set_tab(pid_t pid, int fileno)
{
    pidtab[tabsize] = pid;
    filenotab[tabsize] = fileno;
    tabsize++;
}


static void
unset_tab(int fileno, pid_t *pid)
{
    int i;
    pid_t p;

    for (i = 0; i < tabsize; ++i) {
        if (filenotab[i] == fileno) {
            break;
        }
    }
    if (i == tabsize) {
        exit(EXIT_FAILURE);
    }

    p = pidtab[i];
    pidtab[i] = pidtab[tabsize-1];
    filenotab[i] = filenotab[tabsize-1];
    tabsize--;

    *pid = p;
}


/* Return file stream on success, NULL on failure */
static FILE*
m_popen(char const *command, char const *mode)
{
    int saved_errno, retc;
    int pfd[2];
    pid_t pid;
    char smode;
    FILE *fp;
    int status;

    /* Check whether mode is valid */
    if (mode == NULL || strlen(mode) != 1) {
        errno = EINVAL;
        return NULL;
    }
    switch (mode[0]) {
    case 'r': smode = 'r'; break;
    case 'w': smode = 'w'; break;
    default: errno = EINVAL; return NULL;
    }

    /* Make pipe */
    if (pipe(pfd) == -1) {
        return NULL;
    }

    /* Flush stdout, stderr */
    fflush(stdout);
    fflush(stderr);

    /* fork */
    pid = fork();
    switch (pid) {
    case -1:
        return NULL;
    case 0:
        /* CHILD */
        switch (smode) {
        case 'r':
            if (close(pfd[0]) == -1) {
                exit(EXIT_FAILURE);
            }
            if (dup2(pfd[1], STDOUT_FILENO) == -1) {
                exit(EXIT_FAILURE);
            }
            if (close(pfd[1]) == -1) {
                exit(EXIT_FAILURE);
            }
            status = system(command);
            if (status == -1) {
                exit(EXIT_FAILURE);
            }
            exit(status);
        case 'w':
            if (close(pfd[1]) == -1) {
                exit(EXIT_FAILURE);
            }
            if (dup2(pfd[0], STDIN_FILENO) == -1) {
                exit(EXIT_FAILURE);
            }
            if (close(pfd[0]) == -1) {
                exit(EXIT_FAILURE);
            }
            status = system(command);
            if (status == -1) {
                exit(EXIT_FAILURE);
            }
            exit(status);
        default:
            exit(EXIT_FAILURE);
        }
        break;
    default:
        /* PARENT */
        switch (smode) {
        case 'r':
            if (close(pfd[1]) == -1) {
                goto FAIL;
            }
            fp = fdopen(pfd[0], "r");
            if (fp == NULL) {
                goto FAIL;
            }
            set_tab(pid, pfd[0]);
            return fp;
        case 'w':
            if (close(pfd[0]) == -1) {
                goto FAIL;
            }
            fp = fdopen(pfd[1], "w");
            if (fp == NULL) {
                goto FAIL;
            }
            set_tab(pid, pfd[1]);
            return fp;
        default:
            goto FAIL;
        }
    }

FAIL:
    saved_errno = errno;
    (void) close(pfd[0]);
    (void) close(pfd[1]);
    (void) fclose(fp);
    (void) kill(pid, SIGKILL);
    while ((retc = waitpid(pid, NULL, 0)) > 0) {
        if (errno == EINTR) {
            continue;
        }
        return NULL;
    }
    return NULL;
}


/* Return exit status of child process on success, -1 on failure */
static int
m_pclose(FILE *stream)
{
    int fd;
    pid_t pid;
    int retc, status;

    fd = fileno(stream);
    unset_tab(fd, &pid);

    fflush(stream);
    fclose(stream);
    close(fd);
    while ((retc = waitpid(pid, &status, 0)) > 0) {
        if (errno == EINTR) {
            continue;
        }
        return -1;
    }

    return status;
}


#define BUFSIZE 1024
int
main(int argc __attribute__((unused)),
     char *argv[] __attribute__((unused)))
{
    FILE *fp;
    char buf[BUFSIZE];

    fp = m_popen("ls", "r");
    if (fp == NULL) {
        perror("m_popen");
        exit(EXIT_FAILURE);
    }
    while (fgets(buf, BUFSIZE, fp) != NULL) {
        printf("%s", buf);
    }
    m_pclose(fp);

    fp = m_popen("cat", "w");
    if (fp == NULL) {
        perror("m_popen");
        exit(EXIT_FAILURE);
    }
    fprintf(fp, "---cat---\n");
    m_pclose(fp);

    exit(EXIT_SUCCESS);
}
