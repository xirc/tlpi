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
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ftw.h>


static long n_reg = 0,
            n_dir = 0,
            n_chr = 0,
            n_blk = 0,
            n_link = 0,
            n_fifo = 0,
            n_sock = 0,
            n_unknown = 0;


static void
usage(const char *prog_name, const char *msg)
{
    if (msg != NULL) {
        fprintf(stderr, "%s\n", msg);
    }
    fprintf(stderr, "usage: %s [direcotry-path]\n", prog_name);
    fprintf(stderr, "\t-m use FTW_MOUNT flag\n");
    fprintf(stderr, "\t-p use FTW_PHYS flag\n");
    fflush(stderr);
    exit(EXIT_FAILURE);
}


static int
dir_tree(char const *pathname __attribute__((unused)),
         struct stat const *sbuf,
         int type __attribute__((unused)),
         struct FTW *ftwb __attribute__((unused)))
{
    switch (sbuf->st_mode & S_IFMT) {
        case S_IFREG:  ++n_reg; break;
        case S_IFDIR:  ++n_dir; break;
        case S_IFCHR:  ++n_chr; break;
        case S_IFBLK:  ++n_blk; break;
        case S_IFLNK:  ++n_link; break;
        case S_IFIFO:  ++n_fifo; break;
        case S_IFSOCK: ++n_sock; break;
        default:       ++n_unknown; break;
    }
    return 0;
}


static void
print_stats(FILE *fp)
{
    long n_all = n_reg + n_dir + n_chr +
                 n_blk + n_link + n_fifo + n_unknown;
    fprintf(fp, "All file           : %12ld\n", n_all);
    fprintf(fp, "  Regular file     : %12ld\n", n_reg);
    fprintf(fp, "  Directory        : %12ld\n", n_dir);
    fprintf(fp, "  charactor device : %12ld\n", n_chr);
    fprintf(fp, "  block device     : %12ld\n", n_blk);
    fprintf(fp, "  static link      : %12ld\n", n_link);
    fprintf(fp, "  fifo             : %12ld\n", n_fifo);
    fprintf(fp, "  socket           : %12ld\n", n_sock);
    fprintf(fp, "  UNKNOWN          : %12ld\n", n_unknown);
}


int
main(int argc, char *argv[])
{
    int flags, opt;

    flags = 0;
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
    print_stats(stdout);

    exit(EXIT_SUCCESS);
}
