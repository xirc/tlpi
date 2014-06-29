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
#include <fcntl.h>
#include <sys/stat.h>


int
main(int argc __attribute__((unused)),
     char *argv[] __attribute__((unused)))
{
    int fd;

    if (mkdir("test", S_IRUSR | S_IWUSR | S_IXUSR) == -1) {
        perror("mkdir");
        exit(EXIT_FAILURE);
    }
    if (chdir("test") == -1) {
        perror("chdir");
        exit(EXIT_FAILURE);
    }
    fd =  open("myfile", O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
    if (fd == -1) {
        perror("open");
        exit(EXIT_FAILURE);
    }
    if (symlink("myfile", "../myfile") == -1) {
        perror("symlink");
        exit(EXIT_FAILURE);
    }
    if (chmod("../mylink", S_IRUSR) == -1) {
        perror("chomd");
        fprintf(stderr, "this must be happen!\n");
        exit(EXIT_FAILURE);
    }

    exit(EXIT_SUCCESS);
}
