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


/* i_fcntl_locking.c

   Usage: i_fcntl_locking file...

   where 'file...' is a list of files on which to place locks - the user is
   then prompted to interactively enter commands to test/place locks on
   regions of the files.

   NOTE: The version of the program provided here is an enhanced version
   of that provided in the book. In particular, this version:

       1) handles multiple file name arguments, allowing locks to be
          applied to any of the named files, and
       2) displays information about whether advisory or mandatory
          locking is in effect on each file.
*/


#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>


#define MAX_LINE 100


static void
display_cmd_format(void)
{
    printf("\n    Foramt: cmd lock start length [whence]\n\n");
    printf("    'cmd' is 'g' (GETLK), 's' (SETLK), or 'w' (SETLKW)\n");
    printf("    'lock' is 'r' (READ), 'w' (WRITE), or 'u' (UNLOCK)\n");
    printf("    'start' and 'length' specify byte range to lock\n");
    printf("    'whence' is 's' (SEEK_SET), default), 'c' (SEEK_CUR), "
           "or 'e' (SEEK_END)\n\n");
}


int
main(int argc, char *argv[])
{
    int fd, num_read, cmd, status;
    char lock, cmdch, whence;
    char line[MAX_LINE];
    struct flock fl;
    long long len, start;
    char *p;

    if (argc != 2 || strcmp(argv[1], "--help") == 0) {
        fprintf(stderr, "Usage: %s file\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    fd = open(argv[1], O_RDWR);
    if (fd == -1) {
        fprintf(stderr, "open (%s): %s\n", argv[1], strerror(errno));
        exit(EXIT_FAILURE);
    }

    printf("ENTER ? for help\n");

    while (1) {     /* Prompt for locking command and carry it out */
        printf("PID=%ld> ", (long) getpid());
        fflush(stdout);

        if (fgets(line, MAX_LINE, stdin) == NULL) {     /* EOF */
            break;
        }
        p = strrchr(line, '\n');
        if (p != NULL) {
            /* Remove trailing '\n' */
            *p = '\0';
        }

        if (*line == '\0') {
            /* Skip blank lines */
            continue;
        }

        if (line[0] == '?') {
            display_cmd_format();
            continue;
        }

        whence = 's';       /* In case not otherwise filled in */
        num_read = sscanf(line, "%c %c %lld %lld %c",
                &cmdch, &lock, &start, &len, &whence);
        fl.l_start = start;
        fl.l_len = len;

        if (num_read < 4 ||
            strchr("gsw", cmdch) == NULL ||
            strchr("rwu", lock) == NULL ||
            strchr("sce", whence) == NULL)
        {
            printf("Invalid command\n");
            continue;
        }

        cmd = (cmdch == 'g') ? F_GETLK :
              (cmdch == 's') ? F_SETLK : F_SETLKW;
        fl.l_type = (lock == 'r') ? F_RDLCK :
                    (lock == 'w') ? F_WRLCK : F_UNLCK;
        fl.l_whence = (whence == 'c') ? SEEK_CUR :
                      (whence == 'e') ? SEEK_END : SEEK_SET;

        status = fcntl(fd, cmd, &fl);       /* Perform request... */
        if (cmd == F_GETLK) {
            if (status == -1) {
                perror("fcntl - F_GETLK");
            } else if (fl.l_type == F_UNLCK) {
                printf("[PID=%ld] Lock can be placed\n", (long) getpid());
            } else { /* Locked out by someone else */
                printf("[PID=%ld] Denied by %s lock on %lld:%lld "
                       "(held by PID %ld)\n",  (long) getpid(),
                       (fl.l_type == F_RDLCK) ? "READ" : "WRITE",
                       (long long) fl.l_start,
                       (long long) fl.l_len, (long) fl.l_pid);
            }
        } else {    /* F_SETLK, F_SETLKW */
            if (status == 0) {
                printf("[PID=%ld] %s\n", (long) getpid(),
                        (lock == 'u') ? "unlocked" : "got lock");
            } else if (errno == EAGAIN || errno == EACCES) {    /* F_SETLK */
                printf("[PID=%ld] failed (incompatible lock)\n",
                        (long) getpid());
            } else if (errno == EDEADLK) {                      /* F_SETLKW */
                printf("[PID=%ld] failed (deadlock)\n", (long) getpid());
            } else {
                perror("fcntl - F_SETLK(W)");
            }
        }
    }

    exit(EXIT_SUCCESS);
}
