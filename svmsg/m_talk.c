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
#include <sys/ipc.h>
#include <sys/msg.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <ctype.h>
#include <stddef.h>


#define NAME_MAX 256
#define TEXT_MAX 1024
struct talkmsg {
    long mtype;
    pid_t to;
    pid_t from;
    char name[NAME_MAX];
    char text[TEXT_MAX];
    char __reserved__;
};
#define TALK_MSG_SIZE (offsetof(struct talkmsg, __reserved__) - \
                       offsetof(struct talkmsg, to) + sizeof(char))
#define TALK_MSGQKEY 0x10191936


static int
is_valid_name(char const *name)
{
    char const *p;

    for (p = name; *p != '\0'; ++p) {
        if (isalnum(*p) || *p == '_') {
            continue;
        }
        return -1;
    }
    return 0;
}


static int
is_valid_to(char const *to)
{
    char const *p;

    for (p = to; *p != '\0'; ++p) {
        if (isdigit(*p)) {
            continue;
        }
    }
    return 0;
}


static int
is_valid_text(char const *text)
{
    char const *p;

    for (p = text; *p != '\0'; ++p) {
        if (!iscntrl(*p) && isprint(*p)) {
            continue;
        }
        return -1;
    }
    return 0;
}


int
main(int argc, char *argv[])
{
    int msqid;
    char *name;
    pid_t to;
    struct talkmsg msg;
    int i;
    char *p, *text;
    char buf[TEXT_MAX];

    if (argc != 2 || strcmp(argv[1], "--help") == 0) {
        fprintf(stderr, "Usage: %s name\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    /* Check whether name is valid */
    if (is_valid_name(argv[1]) == -1) {
        fprintf(stderr,
                "'name'(%s) can contains only [a-zA-Z0-9_]\n", argv[1]);
        exit(EXIT_FAILURE);
    }
    name = argv[1];

    /* Create message queue */
    msqid = msgget(TALK_MSGQKEY, IPC_CREAT |
            S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP);   /* rw--w---- */
    if (msqid == -1) {
        perror("msgget");
        exit(EXIT_FAILURE);
    }

    to = 0;
    printf("Hi %s, YOUR ID = %ld\n", name, (long) getpid());
    printf("Type ![user] to send message to 'user'\n");
    while (1) {
        /* Handle received messages */
        for (i = 0; /* do nothing */; ++i) {
            if (msgrcv(msqid, &msg, TALK_MSG_SIZE,
                        getpid(), IPC_NOWAIT | MSG_NOERROR) == -1)
            {
                if (errno == ENOMSG) {
                    break;
                }
                perror("msgrcv");
                exit(EXIT_FAILURE);
            }
            if (is_valid_text(buf) == -1) {
                /* ignore */
                continue;
            }
            if (msg.to != getpid()) {
                /* ignore */
                continue;
            }
            if (i == 0) {
                printf("<<<\n");
            }
            printf("%.*s: %.*s\n",
                    NAME_MAX, msg.name, TEXT_MAX, msg.text);
        }

        /* read user input */
        printf(">>> ");
        fflush(stdout);
        if (fgets(buf, TEXT_MAX, stdin) == NULL) {
            printf("\n");
            break;
        }
        p = strchr(buf, '\n');
        if (p != NULL) {
            *p = '\0';
        }

        /* USER inputs empty string */
        if (buf[0] == '\0') {
            continue;
        }

        if (buf[0] == '!') {
            text = strchr(buf+1, ' ');
            if (text != NULL) {
                *text++ = '\0';
            }
            /* set :to */
            if (is_valid_to(buf + 1) == -1) {
                fprintf(stderr,
                    ":to(pid:%s) can contains only [0-9]\n", buf + 1);
                continue;
            }
            to = strtol(buf + 1, NULL, 10);
            if (text == NULL) {
                printf("Set :to %ld\n", (long) to);
                continue;
            }
        } else {
            text = buf;
        }

        if (to == 0) {
            fprintf(stderr, "Invalid :to %ld\n", (long) to);
            continue;
        }
        msg.mtype = to;
        msg.to = to;
        msg.from = getpid();
        strncpy(msg.name, name, NAME_MAX);
        strncpy(msg.text, text, TEXT_MAX);
        if (text != NULL) {
            if (msgsnd(msqid, &msg, TALK_MSG_SIZE, IPC_NOWAIT) == -1) {
                if (errno == EAGAIN) {
                    fprintf(stderr, "WARNING: message queue is full\n");
                } else {
                    perror("msgsnd");
                }
            }
        }
    }
    printf("Goodby %s\n", name);

    exit(EXIT_FAILURE);
}
