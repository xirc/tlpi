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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "create_pid_file.h"


int
main(int argc, char *argv[])
{
    int fd;
    int sleep_time_s;

    sleep_time_s = 10;
    if (argc > 1) {
        sleep_time_s = strtoul(argv[1], NULL, 0);
    }

    /* Create pid_file and lock it */
    fd = create_pid_file("t_create_pid_file",
            "/tmp/t_create_pid_file.pid", 0);
    if (fd == -1) {
        perror("create_pid_file");
        exit(EXIT_FAILURE);
    }

    /* Do something. */
    sleep(sleep_time_s);

    /* For termination, delete pid file. */
    if (unlink("/tmp/t_create_pid_file.pid") == -1) {
        perror("unlink");
        exit(EXIT_FAILURE);
    }

    exit(EXIT_SUCCESS);
}
