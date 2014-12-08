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


/* view_xattr.c

   Display the extended attributes of a file.

   This program is Linux (2.6 and later) specific.

   See also t_setxattr.c.
*/


#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/xattr.h>


#define XATTR_SIZE 10000


static void
usage(const char *progname)
{
    fprintf(stderr, "usage: %s [-x] file...\n", progname);
    fflush(stderr);
}


int
main(int argc, char *argv[])
{
    char list[XATTR_SIZE], value[XATTR_SIZE];
    ssize_t list_len, value_len;
    int i, j, k, opt, is_hex_disp;

    is_hex_disp = 0;
    while ((opt = getopt(argc, argv, "x")) != -1) {
        switch (opt) {
            case 'x':
                is_hex_disp = 1;
                break;
            default:
                usage(argv[0]);
                exit(EXIT_FAILURE);
        }
    }
    argc -= optind;
    argv += optind;

    for (i = 0; i < argc; ++i) {
        list_len = listxattr(argv[i], list, XATTR_SIZE);
        if (list_len == -1) {
            perror("listxattr");
            exit(EXIT_FAILURE);
        }

        printf("%s: \n", argv[i]);
        for (j = 0; j < list_len; j += (strlen(&list[j]) + 1)) {
            printf("    name=%s; ", &list[j]);

            value_len = getxattr(argv[i], &list[j], value, XATTR_SIZE);
            if (value_len == -1) {
                printf("cannot get value");
            } else if (!is_hex_disp) {
                printf("value=%.*s", (int) value_len, value);
            } else {
                printf("value=");
                for (k = 0; k < value_len; ++k) {
                    printf("%02x ", (unsigned int) value[k]);
                }
            }
            printf("\n");
        }
        printf("\n");
    }

    exit(EXIT_SUCCESS);
}
