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
#include <sys/inotify.h>
#include <limits.h>
#include <ftw.h>
#include <stdarg.h>


/* LIST {{{ */
#define MAX_ENTRY 10000
struct fd_entry {
    int fd;
    char path[PATH_MAX];
};
static struct fd_entry entries[MAX_ENTRY];
static int nentries;
static void
list_init()
{
    int i;
    nentries = 0;
    for (i = 0; i < MAX_ENTRY; ++i) {
        entries[i].fd = -1;
        strcpy(entries[i].path, "");
    }
}
static int
list_insert(struct fd_entry *entry)
{
    int i;
    if (entry == NULL) {
        return -1;
    }
    if (entry->fd == -1) {
        return -1;
    }
    for (i = 0; i < nentries; ++i) {
        if (entries[i].fd == -1) {
            break;
        }
    }
    if (i >= MAX_ENTRY) {
        return -1;
    }
    entries[i].fd = entry->fd;
    strcpy(entries[i].path, entry->path);
    nentries = i + 1 > nentries ? i + 1 : nentries;
    return 0;
}
static int
list_remove(int fd)
{
    int i;
    for (i = 0; i < nentries; ++i) {
        if (entries[i].fd == fd) {
            entries[i].fd = -1;
            return 0;
        }
    }
    return -1;
}
static struct fd_entry*
list_get(int fd)
{
    int i;
    for (i = 0; i < nentries; ++i) {
        if (entries[i].fd == fd) {
            return &entries[i];
        }
    }
    return NULL;
}
/* }}} */


static void display_inotify_event(struct inotify_event *i);

static int inotify_fd;
static int ftw_inotify_watch(
        char const *pathname, struct stat const *sbuf,
        int type, struct FTW *ftwb);


#define BUF_LEN (10 * (sizeof(struct inotify_event) + NAME_MAX + 1))

int
main(int argc, char *argv[])
{
    int wd, j;
    char buf[BUF_LEN];
    ssize_t num_read;
    char *p;
    struct inotify_event *event;

    if (argc < 2 || strcmp(argv[1], "--help") == 0) {
        fprintf(stderr, "%s pathname... \n", argv[0]);
        exit(EXIT_FAILURE);
    }

    inotify_fd = inotify_init();
    if (inotify_fd == -1) {
        perror("inotify_init");
        exit(EXIT_FAILURE);
    }
    list_init();

    for (j = 1; j < argc; ++j) {
        wd = nftw(argv[j], ftw_inotify_watch, 10, /*flags=*/ 0);
        if (wd == -1) {
            fprintf(stderr, "error: nftw\n");
            exit(EXIT_FAILURE);
        }
    }

    wd = 0;
    while (1) {
        num_read = read(inotify_fd, buf, BUF_LEN);
        if (num_read == 0) {
            fprintf(stderr, "read() from inotify fd returned 0!\n");
            exit(EXIT_FAILURE);
        }

        if (num_read == -1) {
            perror("read");
            exit(EXIT_FAILURE);
        }

        printf("Read %ld bytes from inotify fd\n", (long) num_read);

        for (p = buf; p < buf + num_read; /* do nothing */) {
            event = (struct inotify_event *) p;
            display_inotify_event(event);
            if (event->mask & IN_MOVED_FROM &&
                event->mask & IN_ISDIR)
            {
                if (inotify_rm_watch(inotify_fd, event->wd) == -1) {
                    perror("inotify_rm_watch");
                    exit(EXIT_FAILURE);
                }
                if (list_remove(event->wd) == -1) {
                    fprintf(stderr, "list remove error\n");
                    exit(EXIT_FAILURE);
                }
            } else if ((event->mask & IN_MOVED_TO ||
                        event->mask & IN_CREATE) &&
                       event->mask & IN_ISDIR)
            {
                struct fd_entry *entry;
                char newpath[PATH_MAX];
                size_t entry_path_len;
                size_t event_name_len;

                if (event->len <= 0) {
                    fprintf(stderr, "length of name in event <= 0\n");
                    exit(EXIT_FAILURE);
                }
                entry = list_get(event->wd);
                if (entry == NULL) {
                    fprintf(stderr, "list get error\n");
                    exit(EXIT_FAILURE);
                }

                entry_path_len = strlen(entry->path);
                event_name_len = strlen(event->name);
                if (entry_path_len + event_name_len + 2 >= PATH_MAX) {
                    fprintf(stderr, "EXCEEDS PATH_MAX '%s/%s'\n",
                            entry->path, event->name);
                    exit(EXIT_FAILURE);
                }
                strcpy(newpath, entry->path);
                newpath[entry_path_len] = '/';
                strcpy(newpath+entry_path_len+1, event->name);
                newpath[entry_path_len+event_name_len+1] = '\0';
                wd = nftw(newpath, ftw_inotify_watch, 10, /*flags=*/ 0);
                if (wd == -1) {
                    fprintf(stderr, "error: nftw\n");
                    exit(EXIT_FAILURE);
                }
            }
            wd = event->wd;
            p += sizeof(struct inotify_event) + event->len;
        }
    }

    exit(EXIT_SUCCESS);
}


static void
display_inotify_event(struct inotify_event *i)
{
    printf("    wd =%2d; ", i->wd);
    if (i->cookie > 0) {
        printf("cookie =%4d; ", i->cookie);
    }

    printf("mask = ");
#define P(m) do { if (i->mask & m) printf(#m " "); } while(0)
    P(IN_ACCESS);
    P(IN_ATTRIB);
    P(IN_CLOSE_NOWRITE);
    P(IN_CLOSE_WRITE);
    P(IN_CREATE);
    P(IN_DELETE);
    P(IN_DELETE_SELF);
    P(IN_IGNORED);
    P(IN_ISDIR);
    P(IN_MODIFY);
    P(IN_MOVE_SELF);
    P(IN_MOVED_FROM);
    P(IN_MOVED_TO);
    P(IN_OPEN);
    P(IN_Q_OVERFLOW);
    P(IN_UNMOUNT);
#undef P
    printf("\n");

    if (i->len > 0) {
        printf("         name = %s\n", i->name);
    }
}


static int
ftw_inotify_watch(char const *pathname,
                  struct stat const *sbuf __attribute__((unused)),
                  int type,
                  struct FTW *ftwb __attribute__((unused)))
{
    int wd;

    if (type != FTW_D) return 0;
    if (inotify_fd == -1) return -1;

    wd = inotify_add_watch(inotify_fd, pathname, IN_ALL_EVENTS);
    if (wd == -1) {
        perror("inotify_add_watch");
        return -1;
    }
    struct fd_entry entry;
    entry.fd = wd;
    strcpy(entry.path, pathname);
    if (list_insert(&entry) == -1) {
        fprintf(stderr, "list insert error\n");
        return -1;
    }

    printf("FTW: Watching %s using wd %d\n", pathname, wd);

    return 0;
}
