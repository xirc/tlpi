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
#include <string.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/fs.h>


static int chattr(const char *filepath, const char *mode);
static int tom(char m);


int
main(int argc, char *argv[])
{
    int i, failed;

    if (argc < 3 || strcmp(argv[1], "--help") == 0) {
        fprintf(stderr, "usage: %s +-=[ASacDdIijsTtu] files...\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    failed = 0;
    for (i = 2; i < argc; ++i) {
        if (chattr(argv[i], argv[1]) == -1) {
            fprintf(stderr, "failed: 'chattr %s %s'\n", argv[1], argv[i]);
            failed = 1;
        }
    }
    if (failed) {
        exit(EXIT_FAILURE);
    }

    exit(EXIT_SUCCESS);
}


static int
chattr(const char *filepath, const char *mode)
{
    int retv, fd;
    size_t i, len;
    int old, mod, new;

    fd = open(filepath, O_RDONLY);
    if (fd == -1) {
        perror("open");
        return -1;
    }

    if (ioctl(fd, FS_IOC_GETFLAGS, &old) == -1) {
        perror("ioctl(GET)");
        return -1;
    }

    len = strlen(mode);
    mod = 0;
    for (i = 1; i < len; ++i) {
        retv = tom(mode[i]);
        if (retv == -1) {
            fprintf(stderr, "invalid format (%c) in (%s)\n", mode[i], mode);
            return -1;
        }
        mod |= (mode_t)retv;
    }

    switch (mode[0]) {
    case '+':
        new = old | mod;
        break;
    case '-':
        new = old & !mod;
        break;
    case '=':
        new = mod;
        break;
    default:
        fprintf(stderr, "unexpected mode %c (%s)\n", mode[0], mode);
        return -1;
    }

    if (ioctl(fd, FS_IOC_SETFLAGS, &new) == -1) {
        perror("ioctl(SET)");
        return -1;
    }

    return 0;
}


static int
tom(char m)
{
    switch (m) {
        case 'a': return FS_APPEND_FL;
        case 'c': return FS_COMPR_FL;
        case 'D': return FS_DIRSYNC_FL;
        case 'i': return FS_IMMUTABLE_FL;
        case 'j': return FS_JOURNAL_DATA_FL;
        case 'A': return FS_NOATIME_FL;
        case 'd': return FS_NODUMP_FL;
        case 't': return FS_NOTAIL_FL;
        case 's': return FS_SECRM_FL;
        case 'S': return FS_SYNC_FL;
        case 'T': return FS_TOPDIR_FL;
        case 'u': return FS_UNRM_FL;
        default: return -1;
    }
}
