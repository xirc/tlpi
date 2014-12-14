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
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>

#include "semun.h"
#include "binary_sems_fifo.h"


int bs_use_sem_undo = 0;
int bs_retry_on_eintr = 1;


static int
set_blocking_or_not(int fd, int is_blocking)
{
    int flags;

    flags = fcntl(fd, F_GETFL);
    if (flags == -1) {
        return -1;
    }

    if (is_blocking) {
        flags &= ~O_NONBLOCK;
    } else {
        flags |= O_NONBLOCK;
    }
    flags = fcntl(fd, F_SETFL, flags);
    if (flags == -1) {
        return -1;
    }
    return 0;
}


int
init_fifosem(const char *fifo, struct fifosem *fs)
{
    struct stat st;
    fs->rfd = -1;
    fs->wfd = -1;

    fs->rfd = open(fifo, O_RDONLY | O_NONBLOCK);
    if (fs->rfd == -1) {
        goto FAIL;
    }
    if (fstat(fs->rfd, &st) == -1) {
        goto FAIL;
    }
    if (!S_ISFIFO(st.st_mode)) {
        goto FAIL;
    }
    fs->wfd = open(fifo, O_WRONLY | O_NONBLOCK);
    if (fs->wfd == -1) {
        goto FAIL;
    }
    if (fstat(fs->wfd, &st) == -1) {
        goto FAIL;
    }
    if (!S_ISFIFO(st.st_mode)) {
        goto FAIL;
    }

    if (set_blocking_or_not(fs->rfd, /* blocking */ 1) == -1) {
        goto FAIL;
    }
    if (set_blocking_or_not(fs->wfd, /* nonblocking */ 0) == -1) {
        goto FAIL;
    }

    return 0;

FAIL:
    if (fs->rfd != -1) {
        (void) close(fs->rfd);
    }
    if (fs->wfd != -1) {
        (void) close(fs->wfd);
    }
    return -1;
}


/* Initialize semaphore to 1 (i.e., "available") */
int
init_sem_available(struct fifosem *fs)
{
    /* Consume all contents */
    while (reserve_sem_nb(fs) == 0);
    if (errno != EAGAIN) {
        return -1;
    }
    if (write(fs->wfd, "\0", 1) == -1) {
        return -1;
    }
    return 0;
}


/* Initialize semaphore to 0 (i.e., "in use") */
int
init_sem_in_use(struct fifosem *fs)
{
    /* Consume all contents */
    while (reserve_sem_nb(fs) == 0);
    if (errno != EAGAIN) {
        return -1;
    }
    return 0;
}


/* Reserve semaphore (blocking), return 0 on success, or -1 with 'errno'
 * set to EINTR if operation was interrupted by a signal handler */
/* Reserve semaphore - decrement it by 1 */
int
reserve_sem(struct fifosem *fs)
{
    char buf;
    while (read(fs->rfd, &buf, 1) == -1) {
        if (errno != EINTR || !bs_retry_on_eintr) {
            return -1;
        }
    }
    return 0;
}


/* Reserve semaphore - decrement it by 1 */
int
reserve_sem_nb(struct fifosem *fs)
{
    char buf;
    int saved_errno, num_read;

    /* Use NONBLOCKING read */
    if (set_blocking_or_not(fs->rfd, /* nonblocking */ 0) == -1) {
        return -1;
    }

    num_read = read(fs->rfd, &buf, 1);
    saved_errno = errno;

    /* Use BLOCKING read/write */
    if (set_blocking_or_not(fs->rfd, /* blocking */ 1) == -1) {
        return -1;
    }

    errno = saved_errno;
    if (num_read > 0) {
        return 0;
    }
    return -1;
}


/* Release semaphore - increment it by 1 */
int
release_sem(struct fifosem *fs)
{
    if (write(fs->wfd, "\0", 1) == -1) {
        return -1;
    }
    return 0;
}
