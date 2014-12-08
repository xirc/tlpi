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
#include <dirent.h>
#include <limits.h>
#include <sys/stat.h>
#include <stddef.h>


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


static int
my_nftw_iter(const char *dirpath,
        int (*func) (const char *pathname,
                     const struct stat *statbuf,
                     int typeflag,
                     struct FTW *ftwbuf),
        int nopenfd, int flags, int level, int base, int dev)
{
    char path[PATH_MAX];
    int pathlen;
    DIR *dp;
    struct dirent *ent, *result;
    size_t entlen;
    struct stat statbuf, lstatbuf;
    struct FTW ftwbuf;
    int typeflag;
    int retc;
    int dev_id;

    entlen = offsetof(struct dirent, d_name) + NAME_MAX + 1;
    ent = malloc(entlen);
    if (ent == NULL) {
        retc = -1;
        goto EXIT;
    }

    /* typeflag */
    typeflag = FTW_NS;
    if (lstat(dirpath, &lstatbuf) != -1) {
        switch (lstatbuf.st_mode & S_IFMT) {
            case S_IFDIR:
                if (flags & FTW_DEPTH) {
                    typeflag = FTW_DP;
                } else {
                    typeflag = FTW_D;
                }
                break;
            case S_IFLNK:
                typeflag = FTW_SL;
                if (stat(dirpath, &statbuf) == -1) {
                    typeflag = FTW_SLN;
                } else if (!(flags & FTW_PHYS)) {
                    lstatbuf = statbuf;
                    if (S_ISDIR(lstatbuf.st_mode)) {
                        typeflag = FTW_D;
                    } else {
                        typeflag = FTW_F;
                    }
                }
                break;
            default:
                typeflag = FTW_F;
        }
    }
    if (typeflag != FTW_NS &&
        flags & FTW_MOUNT)
    {
        dev_id = lstatbuf.st_dev;
    } else {
        dev_id = -1;
    }
    if (typeflag != FTW_NS &&
        dev != -1 &&
        lstatbuf.st_dev != (unsigned int)dev)
    {
        /* do nothing this directory */
        return 0;
    }

    dp = NULL;
    if (typeflag == FTW_D ||
        typeflag == FTW_DP)
    {
        dp = opendir(dirpath);
        if (dp == NULL) {
            typeflag = FTW_DNR;
        }
    }
    if ((typeflag == FTW_D ||
         typeflag == FTW_DP) &&
        (flags & FTW_CHDIR))
    {
        if (chdir(dirpath) == -1) {
            retc = -1;
            goto EXIT;
        }
    }

    ftwbuf.base = base;
    ftwbuf.level = level;

    if (!(flags & FTW_DEPTH)) {
        retc = func(dirpath, &lstatbuf, typeflag, &ftwbuf);
        if (retc != 0) {
            goto EXIT;
        }
    }

    if (typeflag == FTW_D ||
        typeflag == FTW_DP)
    {
        pathlen = strlen(dirpath);
        while (1) {
            errno = readdir_r(dp, ent, &result);
            if (errno > 0) {
                retc = -1;
                goto EXIT;
            }
            if (result == NULL) {
                break;
            }
            if (strcmp(ent->d_name, ".") == 0) continue;
            if (strcmp(ent->d_name, "..") == 0) continue;

            strcpy(path, dirpath);
            path[pathlen] = '/';
            strcpy(path + pathlen + 1, ent->d_name);
            retc = my_nftw_iter(path, func, nopenfd, flags,
                    level+1, pathlen+1, dev_id);
            if (retc != 0) {
                goto EXIT;
            }
        }
    }

    if (flags & FTW_DEPTH) {
        retc = func(dirpath, &lstatbuf, typeflag, &ftwbuf);
        goto EXIT;
    }

EXIT:
    if (dp != NULL) {
        closedir(dp);
    }
    if (ent != NULL) {
        free(ent);
    }
    return 0;
}


static int
my_nftw(const char *dirpath,
        int (*func) (const char *pathname,
                     const struct stat *statbuf,
                     int typeflag,
                     struct FTW *ftwbuf),
        int nopenfd, int flags)
{
    /* TODO handle nopenfd */
    return my_nftw_iter(dirpath, func, nopenfd, flags, 0, 0, -1);
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

    if (my_nftw((argc > optind) ? argv[optind] : ".", dir_tree, 10, flags) == -1) {
        perror("ntfw");
        exit(EXIT_FAILURE);
    }

    exit(EXIT_SUCCESS);
}
