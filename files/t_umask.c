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


/* t_umask.c

   Demonstrate the affect of umask() in conjunction with open() and mkdir().
*/


#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "file_perms.h"


#define MYFILE "myfile"
#define MYDIR  "mydir"
#define FILE_PERMS    (S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP)
#define DIR_PERMS     (S_IRWXU | S_IRWXG | S_IRWXO)
#define UMASK_SETTING (S_IWGRP | S_IXGRP | S_IWOTH | S_IXOTH)


int
main(int argc __attribute__((unused)),
     char *argv[] __attribute__((unused)))
{
    int fd;
    struct stat sb;
    mode_t u;

    umask(UMASK_SETTING);

    fd = open(MYFILE, O_RDWR | O_CREAT | O_EXCL, FILE_PERMS);
    if (fd == -1) {
        perror("open-" MYFILE);
        exit(EXIT_FAILURE);
    }
    if (mkdir(MYDIR, DIR_PERMS) == -1) {
        perror("mkdir-" MYDIR);
        exit(EXIT_FAILURE);
    }

    u = umask(0);               /* Retrieves (and clears) umask value */

    if (stat(MYFILE, &sb) == -1) {
        perror("stat-" MYFILE);
        exit(EXIT_FAILURE);
    }
    printf("Requested file perms: %s\n", file_perm_str(FILE_PERMS, 0));
    printf("Process umask:        %s\n", file_perm_str(u, 0));
    printf("Actual file perms:    %s\n\n", file_perm_str(sb.st_mode, 0));

    if (stat(MYDIR, &sb) == -1) {
        perror("stat-" MYDIR);
        exit(EXIT_FAILURE);
    }
    printf("Requested dir. perms: %s\n", file_perm_str(DIR_PERMS, 0));
    printf("Process umask:        %s\n", file_perm_str(u, 0));
    printf("Actual dir. perms:    %s\n", file_perm_str(sb.st_mode, 0));

    if (unlink(MYFILE) == -1) {
        perror("unlink-" MYFILE);
        exit(EXIT_FAILURE);
    }
    if (rmdir(MYDIR) == -1) {
        perror("rmdir-" MYDIR);
        exit(EXIT_FAILURE);
    }

    exit(EXIT_SUCCESS);
}
