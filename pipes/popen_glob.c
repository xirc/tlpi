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


/* popen_glob.c

   Demonstrate the use of popen() and pclose().

   This program reads filename wildcard patterns from standard input and
   passes each pattern to a popen() call that returns the output from ls(1)
   for the wildcard pattern. The program displays the returned output.
*/


#define _GNU_SOURCE
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <limits.h>
#include <stdbool.h>
#include <sys/wait.h>


static void print_wait_status(char const *msg, int status);


#define POPEN_FMT "/bin/ls -d %s 2>/dev/null"
#define PAT_SIZE 50
#define PCMD_BUF_SIZE (sizeof(POPEN_FMT) + PAT_SIZE)


int
main(int argc __attribute__((unused)),
     char *argv[] __attribute__((unused)))
{
    char pat[PAT_SIZE];             /* Pattern for globbing */
    char popen_cmd[PCMD_BUF_SIZE];
    FILE *fp;                       /* File stream returned by popen() */
    bool bad_pattern;               /* Invalid characters in 'pat'? */
    int len, status, file_cnt, j;
    char pathname[PATH_MAX];

    /* Read pattern, display results of globbing */
    while (1) {
        printf("pattern: ");
        fflush(stdout);
        if (fgets(pat, PAT_SIZE, stdin) == NULL) {
            /* EOF */
            break;
        }
        len = strlen(pat);
        if (len <= 1) {
            /* Empty line */
            continue;
        }

        if (pat[len - 1] == '\n') {   /* Strip trailing newline */
            pat[len - 1] = '\0';
        }
        /* Ensure that the pattern contains only valid characters,
         * i.e., letters, digits, underscore, dot, and the shell
         * globbing characters. (Our definition of valid is more
         * restrictive than the shell, which permits other characters
         * to be included in a filename if they are quoted.) */
        for (j = 0, bad_pattern = false; j < len && !bad_pattern; ++j) {
            if (!isalnum((unsigned char) pat[j]) &&
                    strchr("_*?[^-].", pat[j]) == NULL)
            {
                bad_pattern = true;
            }
        }
        if (bad_pattern) {
            printf("Bad pattern character: %c\n", pat[j - 1]);
            continue;
        }

        /* Build and execute command to glob 'pat' */
        snprintf(popen_cmd, PCMD_BUF_SIZE, POPEN_FMT, pat);
        fp = popen(popen_cmd, "r");
        if (fp == NULL) {
            printf("popen() failed\n");
            continue;
        }

        /* Read resulting list of pathnames until EOF */
        file_cnt = 0;
        while (fgets(pathname, PATH_MAX, fp) != NULL) {
            printf("%s", pathname);
            file_cnt++;
        }

        /* Close pipe, fetch and display termination status */
        status = pclose(fp);
        printf("    %d matching file%s\n",
                file_cnt, (file_cnt != 1) ? "s" : "");
        printf("    pclose() status == %#x\n", (unsigned int) status);
        if (status != -1) {
            print_wait_status("\t", status);
        }
    }

    exit(EXIT_SUCCESS);
}


static void
print_wait_status(char const *msg, int status)
{
    if (msg != NULL) {
        printf("%s", msg);
    }

    if (WIFEXITED(status)) {
        printf("child exited, status=%d\n", WEXITSTATUS(status));
    } else if (WIFSIGNALED(status)) {
        printf("child killed by signal %d (%s)",
                WTERMSIG(status), strsignal(WTERMSIG(status)));
        if (WCOREDUMP(status)) {
            printf(" (core dumped)");
        }
        printf("\n");
    } else if (WIFSTOPPED(status)) {
        printf("child stopped by signal %d (%s)\n",
                WSTOPSIG(status), strsignal(WSTOPSIG(status)));
    } else if (WIFCONTINUED(status)) {
        printf("child continued\n");
    } else {
        /* Should never happen */
        printf("what happend to this child? (status=%x)\n",
                (unsigned int) status);
    }
}
