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
#include <ctype.h>
#include <stddef.h>
#include <mqueue.h>


#define NAME_MAX 256
#define TEXT_MAX 1024
struct talkmsg {
    pid_t to;
    pid_t from;
    char name[NAME_MAX];
    char text[TEXT_MAX];
};
#define TALK_MSGQ_TEMPLATE "/talk_cli.%ld"
#define TALK_MSGQ_SIZE (sizeof(TALK_MSGQ_TEMPLATE) + 20)


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
    mqd_t pmqd, to_pmqd;
    char pmsgq[TALK_MSGQ_SIZE];
    char to_pmsgq[TALK_MSGQ_SIZE];
    struct mq_attr attr;

    char *name;
    pid_t to;
    struct talkmsg msg;
    int i;
    char *p, *text;
    char buf[TEXT_MAX];
    ssize_t num_read;

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
    attr.mq_maxmsg = 8;
    attr.mq_msgsize = sizeof(struct talkmsg);
    snprintf(pmsgq, TALK_MSGQ_SIZE, TALK_MSGQ_TEMPLATE, (long) getpid());
    pmqd = mq_open(pmsgq, O_RDWR | O_CREAT | O_EXCL | O_NONBLOCK,
            S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP, &attr);
    if (pmqd == -1) {
        perror("mq_open");
        exit(EXIT_FAILURE);
    }

    to = 0;
    printf("Hi %s, YOUR ID = %ld\n", name, (long) getpid());
    printf("Type ![user] to send message to 'user'\n");
    while (1) {
        /* Handle received messages */
        for (i = 0; /* do nothing */; ++i) {
            num_read =
                mq_receive(pmqd, (char*)&msg, sizeof(struct talkmsg), NULL);
            if (num_read != sizeof(struct talkmsg))
            {
                if (num_read == -1 && errno == EAGAIN) {
                    break;
                }
                perror("mq_receive");
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

        msg.to = to;
        msg.from = getpid();
        strncpy(msg.name, name, NAME_MAX);
        strncpy(msg.text, text, TEXT_MAX);
        if (text != NULL) {
            snprintf(to_pmsgq, TALK_MSGQ_SIZE, TALK_MSGQ_TEMPLATE, (long) to);
            to_pmqd = mq_open(to_pmsgq, O_WRONLY);
            if (to_pmqd == -1) {
                perror("mq_open");
                continue;
            }

            if (mq_send(to_pmqd, (char*)&msg,
                        sizeof(struct talkmsg), 0) == -1)
            {
                if (errno == EAGAIN) {
                    fprintf(stderr, "WARNING: message queue is full\n");
                } else {
                    perror("mq_send");
                }
            }

            if (mq_close(to_pmqd) == -1) {
                perror("mq_close");
            }
        }
    }
    printf("Goodby %s\n", name);

    if (mq_unlink(pmsgq) == -1) {
        perror("mq_unlink");
        exit(EXIT_FAILURE);
    }
    if (mq_close(pmqd) == -1) {
        perror("mq_close");
        exit(EXIT_FAILURE);
    }

    exit(EXIT_FAILURE);
}
