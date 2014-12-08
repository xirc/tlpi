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


#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <ftw.h>


static void
usage(const char *prog_name, const char *msg)
{
    if (msg != NULL) {
        fprintf(stderr, "%s\n", msg);
    }
    fprintf(stderr, "usage: %s [-dmp] [direcotry-path]\n", prog_name);
    fprintf(stderr, "\t-m use FTW_MOUNT flag\n");
    fprintf(stderr, "\t-p use FTW_PHYS flag\n");
    fflush(stderr);
    exit(EXIT_FAILURE);
}


static int
dir_tree(char const *pathname, struct stat const *sbuf,
         int type, struct FTW *ftwb __attribute__((unused)))
{
    switch (sbuf->st_mode & S_IFMT) {
        case S_IFREG:  printf("-"); break;
        case S_IFDIR:  printf("d"); break;
        case S_IFCHR:  printf("c"); break;
        case S_IFBLK:  printf("b"); break;
        case S_IFLNK:  printf("l"); break;
        case S_IFIFO:  printf("p"); break;
        case S_IFSOCK: printf("s"); break;
        default:       printf("?"); break;
    }

    printf(" %s  ",
            (type == FTW_D)   ? "D  " :
            (type == FTW_DNR) ? "DNR" :
            (type == FTW_DP)  ? "DP " :
            (type == FTW_F)   ? "F  " :
            (type == FTW_SL)  ? "SL " :
            (type == FTW_SLN) ? "SLN" :
            (type == FTW_NS)  ? "NS " : "   ");

    if (type != FTW_NS) {
        printf("%7ld", (long) sbuf->st_ino);
    } else {
        printf("%*c", 7, ' ');
    }

    printf(" %s\n", pathname);

    return 0;
}


int
main(int argc, char *argv[])
{
    int flags, opt;

    flags = FTW_DEPTH;
    while ((opt = getopt(argc, argv, "dmp")) != -1) {
        switch (opt) {
        case 'm':
            flags |= FTW_MOUNT;
            break;
        case 'p':
            flags |= FTW_PHYS;
            break;
        default:
            usage(argv[0], NULL);
        }
    }

    if (argc > optind + 1) {
        usage(argv[0], NULL);
    }

    if (nftw((argc > optind) ? argv[optind] : ".",
                dir_tree, 10, flags) == -1)
    {
        perror("ntfw");
        exit(EXIT_FAILURE);
    }

    exit(EXIT_SUCCESS);
}
