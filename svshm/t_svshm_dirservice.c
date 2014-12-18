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
#include <stdarg.h>

#include "svshm_dirservice.h"


static void
usage(char const *progname)
{
    fprintf(stdout, "Usage: %s {cmxrw} [...]]\n", progname);
    fprintf(stdout, "    s                           create sem & shm\n");
    fprintf(stdout, "    e                           delete sem & shm\n");
    fprintf(stdout, "    c    path       value       create new entry\n");
    fprintf(stdout, "    m    old-path   new-path    move the entry\n");
    fprintf(stdout, "    x    path                   delete the entry\n");
    fprintf(stdout, "    r    path                   read the value of entry\n");
    fprintf(stdout, "    w    path       new-value   write the value of entry\n");
    fprintf(stdout, "    l                           list all entries\n");
}


static int
toint(char const *str, char const *name)
{
    int val;
    char *endptr;

    /* empty string */
    if (str == NULL || str[0] == '\0') {
        goto FAIL;
    }

    errno = 0;
    val = strtol(str, &endptr, 0);
    if (errno != 0) {
        goto FAIL;
    }
    if (*endptr != '\0') {
        goto FAIL;
    }

    return val;

FAIL:
    fprintf(stderr, "Invalid number %s\n", name);
    exit(EXIT_FAILURE);
}


static void
chkargc(int argc, int expect) {
    if (argc == expect) {
        return;
    }
    fprintf(stderr, "# of args: expect %d, but given %d\n", expect, argc);
    exit(EXIT_FAILURE);
}


int
main(int argc,
     char *argv[])
{
    int retc;
    int is_setup, is_cleanup;
    void const *value;
    void const *old_value;
    void const *new_value;
    int nentries;
    struct shment *entry;

    /* Check arguments */
    if (argc < 2 || strcmp(argv[1], "--help") == 0) {
        usage(argv[0]);
        exit(EXIT_FAILURE);
    }
    if (strlen(argv[1]) != 1) {
        fprintf(stderr,
                "\"%s\" should be 1 character {s|e|c|m|x|r|w}\n", argv[1]);
        exit(EXIT_FAILURE);
    }
    switch (argv[1][0]) {
    case 's':   /* fall through */
    case 'e':   /* fall through */
    case 'l':   chkargc(argc - 2, 0); break;
    case 'c':   /* fall through */
    case 'w':   /* fall through */
    case 'm':   chkargc(argc - 2, 2); break;
    case 'x':   /* fall through */
    case 'r':   chkargc(argc - 2, 1); break;
    default:
        fprintf(stderr, "%s is not in {cmxrw}.\n", argv[1]);
        exit(EXIT_FAILURE);
    }

    /* Setup or Cleanup */
    is_setup = argv[1][0] == 's';
    is_cleanup = argv[1][0] == 'e';

    /* Setup */
    retc = shment_setup(is_setup ? 1 : 0);
    /* Ignore error if we want to cleanup */
    if (!is_cleanup && retc == -1) {
        perror("shment_setup");
        exit(EXIT_FAILURE);
    }
    if (is_setup) {
        exit(EXIT_SUCCESS);
    }

    /* Handle each sub-command */
    switch (argv[1][0]) {
    case 'c':
        value = (void*)(long)toint(argv[3], "value");
        retc = shment_create(argv[2], value);
        if (retc == -1) {
            if (errno != 0) {
                perror("shment_create");
            } else {
                fprintf(stderr, "Entry exists already, or table is full.\n");
            }
            exit(EXIT_FAILURE);
        }
        printf("[CREATED] %s: %ld\n", argv[2], (long)value);
        break;
    case 'm':
        retc = shment_move(argv[2], argv[3]);
        if (retc == -1) {
            if (errno != 0) {
                perror("shment_move");
            } else {
                fprintf(stderr, "Source entry does not exists, or\n"
                        "destination entry exists already.\n");
            }
            exit(EXIT_FAILURE);
        }
        printf("[MOVED] %s -> %s\n", argv[2], argv[3]);
        break;
    case 'x':
        retc = shment_remove(argv[2], &old_value);
        if (retc == -1) {
            if (errno != 0) {
                perror("shment_remove");
            } else {
                fprintf(stderr, "The entry does not exists.\n");
            }
            exit(EXIT_FAILURE);
        }
        printf("[DELETED] %s: %ld\n", argv[2], (long)old_value);
        break;
    case 'r':
        retc = shment_ctl_value(argv[2], NULL, &old_value);
        if (retc == -1) {
            if (errno != 0) {
                perror("shment_ctl_value");
            } else {
                fprintf(stderr, "The entry does not exists.\n");
            }
            exit(EXIT_FAILURE);
        }
        printf("[READ] %s: %ld\n", argv[2], (long)old_value);
        break;
    case 'w':
        new_value = (void*)(long)toint(argv[3], "new-value");
        retc = shment_ctl_value(argv[2], &new_value, &old_value);
        if (retc == -1) {
            if (errno != 0) {
                perror("shment_ctl_value");
            } else {
                fprintf(stderr, "The entry does not exists.\n");
            }
            exit(EXIT_FAILURE);
        }
        printf("[WROTE] %s: %ld -> %s: %ld\n",
                argv[2], (long)old_value,
                argv[2], (long)new_value);
        break;
    case 'l':
        nentries = 0;
        while ((entry = shment_get()) != NULL) {
            printf("%s: %ld\n", entry->path, (long)entry->value);
            nentries++;
        }
        shment_end();
        if (nentries == 0) {
            printf("No entries available.\n");
        } else {
            printf("%d entries available.\n", nentries);
        }
        break;
    }

    /* Cleanup */
    retc = shment_cleanup(is_cleanup ? 1 : 0);
    if (retc == -1) {
        perror("shment_cleanup");
        exit(EXIT_FAILURE);
    }

    exit(EXIT_SUCCESS);
}
