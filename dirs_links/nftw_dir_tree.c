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


/* nftw_dir_tree.c

   Demonstrate the use of nftw(3). Walk though the directory tree specified
   on the command line (or the current working directory if no directory
   is specified on the command line), displaying an indented hierarchy
   of files in the tree. For each file, display:

      * a letter indicating the file type (using the same letters
        as "ls -l"), as obtained using stat(2);
      * a string indicating the file type, as supplied by nftw(); and
      * the file's i-node number.
*/


#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ftw.h>


static void
usage(const char *prog_name, const char *msg)
{
    if (msg != NULL) {
        fprintf(stderr, "%s\n", msg);
    }
    fprintf(stderr, "usage: %s [-dmp] [direcotry-path]\n", prog_name);
    fprintf(stderr, "\t-d use FTW_DEPTH flag\n");
    fprintf(stderr, "\t-m use FTW_MOUNT flag\n");
    fprintf(stderr, "\t-p use FTW_PHYS flag\n");
    fflush(stderr);
    exit(EXIT_FAILURE);
}


static int
dir_tree(char const *pathname, struct stat const *sbuf,
         int type, struct FTW *ftwb)
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

    printf(" %*s", 4 * ftwb->level, "");
    printf("%s\n", &pathname[ftwb->base]);

    return 0;
}


int
main(int argc, char *argv[])
{
    int flags, opt;

    flags = 0;
    while ((opt = getopt(argc, argv, "dmp")) != -1) {
        switch (opt) {
        case 'd':
            flags |= FTW_DEPTH;
            break;
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
