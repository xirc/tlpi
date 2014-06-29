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
#include <sys/acl.h>
#include <acl/libacl.h>
#include <sys/stat.h>
#include <pwd.h>
#include <grp.h>


static uid_t uid_from_uname(char const *uname);
static gid_t gid_from_gname(char const *gname);
static char * uname_from_uid(uid_t const uid);
static char * gname_from_gid(gid_t const gid);
static void print_acl_entry(acl_entry_t entry, acl_entry_t mask);


static void
usage(const char *progname)
{
    fprintf(stderr, "Usage: %s (u|g) (id_or_name) file\n", progname);
    fflush(stderr);
}


int
main(int argc, char *argv[])
{
    struct stat s;
    int is_uid, is_gid;
    uid_t uid;
    gid_t gid;
    int eid, pri;
    acl_t acl;
    acl_type_t type;
    acl_entry_t entry, saved_entry, mask_entry;
    acl_tag_t tag;
    uid_t *uidp;
    gid_t *gidp;

    if (argc != 4 || strcmp(argv[1], "--help") == 0) {
        usage(argv[0]);
        exit(EXIT_FAILURE);
    }

    /* uid or gid */
    if (strcmp(argv[1], "u") == 0) {
        is_uid = 1;
        is_gid = 0;
        uid = uid_from_uname(argv[2]);
        if ((long)uid == -1) {
            fprintf(stderr, "'%s' is not usr_name or usr_id\n", argv[2]);
            exit(EXIT_FAILURE);
        }
    } else if (strcmp(argv[1], "g") == 0) {
        is_uid = 0;
        is_gid = 1;
        gid = gid_from_gname(argv[2]);
        if ((long)gid == -1) {
            fprintf(stderr, "'%s' is not grp_name or grp_id\n", argv[2]);
            exit(EXIT_FAILURE);
        }
    } else {
        fprintf(stderr, "'%s' should be 'u' or 'g'\n", argv[1]);
        usage(argv[0]);
        exit(EXIT_FAILURE);
    }

    /* file stat */
    if (stat(argv[3], &s) == -1) {
        perror("stat");
        exit(EXIT_FAILURE);
    }
    switch (s.st_mode & S_IFMT) {
    case S_IFREG:
        type = ACL_TYPE_ACCESS;
        break;
    case S_IFDIR:
        type = ACL_TYPE_DEFAULT;
        break;
    default:
        fprintf(stderr, "%s is not REGULAR FILE or DIRECTORY\n", argv[3]);
        exit(EXIT_SUCCESS);
    }

    /* acl */
    acl = acl_get_file(argv[3], type);
    if (acl == NULL) {
        perror("acl_get_file");
        exit(EXIT_FAILURE);
    }
    if (acl_calc_mask(&acl) == -1) {
        perror("acl_calc_mask");
        exit(EXIT_FAILURE);
    }
    pri = 0xFF;
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

        if (tag == ACL_USER_OBJ &&
            (is_uid && s.st_uid == uid))
        {
            pri = 0;
            saved_entry = entry;
        }

        if (tag == ACL_USER &&
            pri > 1 &&
            is_uid)
        {
            uidp = acl_get_qualifier(entry);
            if (uidp == NULL) {
                perror("acl_get_qualifier");
                exit(EXIT_FAILURE);
            }
            if (uid == *uidp) {
                pri = 1;
                saved_entry = entry;
            }
           if (acl_free(uidp) == -1) {
                perror("acl_free");
                exit(EXIT_FAILURE);
            }
        }

        if (tag == ACL_GROUP_OBJ &&
            pri > 2 &&
            is_gid &&
            s.st_gid == gid)
        {
            pri = 2;
            saved_entry = entry;
        }

        if (tag == ACL_GROUP &&
            pri > 3 &&
            is_gid)
        {
            gidp = acl_get_qualifier(entry);
            if (gidp == NULL) {
                perror("acl_get_qualifier");
                exit(EXIT_FAILURE);
            }
            if (gid == *gidp) {
                pri = 3;
                saved_entry = entry;
            }
            if (acl_free(gidp) == -1) {
                perror("acl_free");
                exit(EXIT_FAILURE);
            }
        }

        if (tag == ACL_MASK) {
            mask_entry = entry;
        }

        if (tag == ACL_OTHER &&
            pri > 4)
        {
            pri = 4;
            saved_entry = entry;
        }
    }
    if (pri != 0xFF) {
        print_acl_entry(saved_entry, mask_entry);
    }
    if (acl_free(acl) == -1) {
        perror("acl_free");
        exit(EXIT_FAILURE);
    }

    exit(EXIT_SUCCESS);
}


static uid_t
uid_from_uname(char const *uname)
{
    struct passwd *pwd;

    pwd = getpwnam(uname);
    if (pwd == NULL) {
        return -1;
    }
    return pwd->pw_uid;
}


static gid_t
gid_from_gname(char const *gname)
{
    struct group *grp;

    grp = getgrnam(gname);
    if (grp == NULL) {
        return -1;
    }
    return grp->gr_gid;
}


static char *
uname_from_uid(uid_t const uid)
{
    struct passwd *usr;

    usr = getpwuid(uid);
    if (usr == NULL) {
        return NULL;
    }
    return usr->pw_name;
}


static char *
gname_from_gid(gid_t const gid)
{
    struct group *grp;

    grp = getgrgid(gid);
    if (grp == NULL) {
        return NULL;
    }
    return grp->gr_name;
}


static void
print_acl_entry(acl_entry_t entry, acl_entry_t mask)
{
    acl_tag_t tag;
    acl_permset_t permset, mask_permset;
    uid_t *uidp;
    gid_t *gidp;
    char *name;
    int permval, maskval, is_mask;

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

    if (tag == ACL_USER) {
        uidp = acl_get_qualifier(entry);
        if (uidp == NULL) {
            perror("acl_getqualifier");
            exit(EXIT_FAILURE);
        }
        name = uname_from_uid(*uidp);
        if (name == NULL) {
            printf("%-8d", *uidp);
        } else {
            printf("%-8s", name);
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
        name = gname_from_gid(*gidp);
        if (name == NULL) {
            printf("%-8d", *gidp);
        } else {
            printf("%-8s", name);
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
    is_mask = (tag == ACL_USER || tag == ACL_GROUP || tag == ACL_GROUP_OBJ);
    if (is_mask &&
        acl_get_permset(mask, &mask_permset) == -1)
    {
        perror("acl_get_permset");
        exit(EXIT_FAILURE);
    }

#define PUTC_PERM(ACL, c) \
    do { \
        permval = acl_get_perm(permset, ACL); \
        if (permval == -1) { \
            fprintf(stderr, "%s - " #ACL, strerror(errno)); \
        } \
        maskval = 1; \
        if (is_mask){ \
            maskval = acl_get_perm(mask_permset, ACL); \
            if (maskval == -1) { \
                fprintf(stderr, "%s - " #ACL, strerror(errno)); \
            } \
        } \
        printf("%c", (permval == 1 && maskval == 1) ? c : '-'); \
    } while (0);

    PUTC_PERM(ACL_READ, 'r');
    PUTC_PERM(ACL_WRITE, 'w');
    PUTC_PERM(ACL_EXECUTE, 'x');
    printf("\n");

#undef PUTC_PERM
}
