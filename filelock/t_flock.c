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


/* t_flock.c

   Demonstrate the use of flock() to place file locks.
*/


#include <sys/types.h>
#include <sys/file.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <time.h>
#include <string.h>


static char *
now(const char *format)
{
#define BUFSIZE 1024
    static char buf[BUFSIZE];
    time_t t;
    struct tm *tm;

    t = time(NULL);
    if (t == (time_t)-1) {
        return NULL;
    }

    tm = localtime(&t);
    if (tm == NULL) {
        return NULL;
    }

    (void) strftime(buf, BUFSIZE, format == NULL ? "%T" : format, tm);
    return buf;
}


static void
usage(const char *progname)
{
    printf("Usage: %s file lock [sleep-time]\n", progname);
    printf("    'lock' is 's' (shared) or 'x' (exclusive)\n");
    printf("        optionally followed by 'n' (nonblocking)\n");
    printf("    'secs' specifies time to hold lock\n");
}


int
main(int argc, char *argv[])
{
    int fd, lock;
    const char *lname;
    unsigned int sleep_time;

    if (argc == 2 && strcmp(argv[1], "--help") == 0) {
        usage(argv[0]);
        exit(EXIT_SUCCESS);
    }
    if (argc < 3 || strchr("sx", argv[2][0]) == NULL) {
        usage(argv[0]);
        exit(EXIT_FAILURE);
    }

    lock = (argv[2][0] == 's') ? LOCK_SH : LOCK_EX;
    if (argv[2][1] == 'n') {
        lock |= LOCK_NB;
    }

    /* Open file to be locked */
    fd = open(argv[1], O_RDONLY);
    if (fd == -1) {
        perror("open");
        exit(EXIT_FAILURE);
    }

    lname = (lock & LOCK_SH) ? "LOCK_SH" : "LOCK_EX";
    printf("PID %ld: requesting %s at %s\n",
            (long) getpid(), lname, now("%T"));

    if (flock(fd, lock) == -1) {
        if (errno == EWOULDBLOCK) {
            fprintf(stderr,
                    "PID %ld: already locked - bye!\n", (long) getpid());
            exit(EXIT_FAILURE);
        } else {
            fprintf(stderr,
                    "flock (PID=%ld): %s\n", (long) getpid(), strerror(errno));
            exit(EXIT_FAILURE);
        }
    }

    printf("PID %ld: granted    %s at %s\n",
            (long) getpid(), lname, now("%T"));

    sleep_time = (argc > 3) ? strtoul(argv[3], NULL, 0) : 10;
    sleep(sleep_time);

    printf("PID %ld: releasing  %s at %s\n",
            (long) getpid(), lname, now("%T"));

    if (flock(fd, LOCK_UN) == -1) {
        perror("flock");
        exit(EXIT_FAILURE);
    }

    exit(EXIT_SUCCESS);
}
