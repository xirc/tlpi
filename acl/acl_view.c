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


/* acl_view.c

   Display the access control list (ACL) on a file.

   Usage: acl_view [-d] file

   If the '-d' option is specified, then the default ACL is displayed (and
   'file' must be a directory), otherwise the access ACL is displayed.

   This program is Linux-specific. ACLs are supported since Linux 2.6.
   To build this program, you must have the ACL library (libacl) installed
   on your system.
*/


#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/acl.h>
#include <acl/libacl.h>
#include <pwd.h>
#include <grp.h>
#include <string.h>


static void
usage(const char *progname)
{
    fprintf(stderr, "usage: %s [-d] file\n", progname);
    fflush(stderr);
}


int
main(int argc, char *argv[])
{
    int opt, eid, permval;
    acl_t acl;
    acl_type_t type;
    acl_entry_t entry;
    acl_tag_t tag;
    acl_permset_t permset;
    uid_t *uidp;
    gid_t *gidp;
    struct passwd *usr;
    struct group *grp;

    type = ACL_TYPE_ACCESS;
    while ((opt = getopt(argc, argv, "d")) != -1) {
        switch (opt) {
        case 'd':
            type = ACL_TYPE_DEFAULT;
            break;
        default:
            usage(argv[0]);
            exit(EXIT_FAILURE);
        }
    }

    if (optind + 1 != argc) {
        usage(argv[0]);
        exit(EXIT_FAILURE);
    }

    acl = acl_get_file(argv[optind], type);
    if (acl == NULL) {
        perror("acl_get_file");
        exit(EXIT_FAILURE);
    }

    for (eid = ACL_FIRST_ENTRY; /*true*/; eid = ACL_NEXT_ENTRY) {
        if (acl_get_entry(acl, eid, &entry) != 1) {
            /* exit on error or no more entries */
            break;
        }

        /* retrieve and display tag type */
        if (acl_get_tag_type(entry, &tag) == -1) {
            perror("acl_get_tag_type");
            exit(EXIT_FAILURE);
        }
        printf("%-12s",
                (tag == ACL_USER_OBJ) ? "user_obj" :
                (tag == ACL_USER) ? "user" :
                (tag == ACL_GROUP_OBJ) ? "group_obj" :
                (tag == ACL_GROUP) ? "group" :
                (tag == ACL_MASK) ? "mask" :
                (tag == ACL_OTHER) ? "other" : "???");

        /* retrieve and display optional tag quolifier */
        if (tag == ACL_USER) {
            uidp = acl_get_qualifier(entry);
            if (uidp == NULL) {
                perror("acl_getqualifier");
                exit(EXIT_FAILURE);
            }

            usr = getpwuid(*uidp);
            if (usr == NULL) {
                printf("%-8d", *uidp);
            } else {
                printf("%-8s", usr->pw_name);
            }
            if (acl_free(uidp) == -1) {
                perror("acl_free");
                exit(EXIT_FAILURE);
            }
        } else if (tag == ACL_GROUP) {
            gidp = acl_get_qualifier(entry);
            if (gidp == NULL) {
                perror("acl_get_qualifier");
                exit(EXIT_FAILURE);
            }
            grp = getgrgid(*gidp);
            if (grp == NULL) {
                printf("%-8d", *gidp);
            } else {
                printf("%-8s", grp->gr_name);
            }
            if (acl_free(gidp) == -1) {
                perror("acl_free");
                exit(EXIT_FAILURE);
            }
        } else {
            printf("        ");
        }

        /* retrieve and display permissions */
        if (acl_get_permset(entry, &permset) == -1) {
            perror("acl_get_permset");
            exit(EXIT_FAILURE);
        }

#define PUTC_PERM(ACL, c) \
        do { \
            permval = acl_get_perm(permset, ACL); \
            if (permval == -1) { \
                fprintf(stderr, "%s - " #ACL, strerror(errno)); \
            } \
            printf("%c", (permval == 1) ? c : '-'); \
        } while (0);

        PUTC_PERM(ACL_READ, 'r');
        PUTC_PERM(ACL_WRITE, 'w');
        PUTC_PERM(ACL_EXECUTE, 'x');
        printf("\n");
#undef PUTC_PERM
    }

    if (acl_free(acl) == -1) {
        perror("acl_free");
        exit(EXIT_FAILURE);
    }

    exit(EXIT_SUCCESS);
}
