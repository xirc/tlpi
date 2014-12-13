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
#include <stdbool.h>
#include <string.h>
#include <pthread.h>
#include <assert.h>
#include "ubt.h"


/* KEY SYSTEM {{{ */
#define NCHUNKS 1000
#define KEYLEN    10
static char chunks[NCHUNKS];
static const size_t nchunks = NCHUNKS;
static const size_t keylen = KEYLEN;
static const size_t nkeys = NCHUNKS / KEYLEN;


static void
init_keys()
{
    unsigned int seed;
    unsigned int i;

    seed = (unsigned int)time(NULL);
    for (i = 0; i < nchunks; ++i) {
        chunks[i] = 'a' + rand_r(&seed) % 26;
        if (i % keylen == keylen-1) {
            chunks[i] = '\0';
        }
    }
    chunks[nchunks-1] = '\0';
}


static char *
get_key(unsigned int index)
{
    if (index > nkeys) {
        return NULL;
    }
    return &chunks[index * keylen];
}
/* KEY SYSTEM }}} */


static struct Node tree;


static void
pexit(char const *msg, int s)
{ /* {{{ */
    fprintf(stderr, "%s (errmsg=%s, errno=%d)\n", msg, strerror(s), s);
    exit(EXIT_FAILURE);
} /* }}} */


static void *
thread_adder(void *arg)
{ /* {{{ */
    unsigned int seed;
    int i, j, s, loopn, addn;
    char *key;

    seed = 0;
    loopn = *(int*)arg;
    addn = 0;
    for (i = 0; i < loopn; ++i) {
        j = rand_r(&seed) % nkeys;
        key = get_key(j);
        s = add(&tree, key, (void*)j);
        if (s == 0) {
            addn++;
        } else if (s != -1) {
            pexit("add@tree", s);
        }
    }
    printf("ADD %d\n", addn);

    return NULL;
} /* }}} */


static void *
thread_deler(void *arg)
{ /* {{{ */
    unsigned int seed;
    int i, j, s, loopn, deln;
    char *key;

    seed = 1;
    loopn = *(int*)arg;
    deln = 0;
    for (i = 0; i < loopn; ++i) {
        j = rand_r(&seed) % nkeys;
        key = get_key(j);
        s = delete(&tree, key);
        if (s == 0) {
            deln++;
        } else if (s != -1) {
            pexit("delete@tree", s);
        }
    }

    printf("DELETE %d\n", deln);

    return NULL;
} /* }}} */


static void *
thread_looker(void *arg)
{ /* {{{ */
    unsigned int seed;
    int i, j, s, loopn, lookn;
    char *key;
    void *value;

    seed = 2;
    loopn = *(int*)arg;
    lookn = 0;
    for (i = 0; i < loopn; ++i) {
        j = rand_r(&seed) % nkeys;
        key = get_key(j);
        s = lookup(&tree, key, &value);
        if (s == 0) {
            lookn++;
        } else if (s != -1) {
            pexit("delete@tree", s);
        }
    }

    printf("LOOKUP %d\n", lookn);

    return NULL;
} /* }}} */


int
main(int argc, char *argv[])
{
    pthread_t th_adder, th_deler, th_looker;
    int s, maxn;

    if (argc == 2 && strcmp(argv[1], "--help") == 0) {
        fprintf(stderr, "usage: %s [ntry]\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    maxn = (argc > 1) ? atoi(argv[1]) : 1000000;
    if (maxn < 0) {
        fprintf(stderr, "ntry %s > 0\n", argv[1]);
        exit(EXIT_FAILURE);
    }

    /* INITIALIZE TREE & KEY */
    s = initialize(&tree);
    assert(s == 0);
    init_keys();

    s = pthread_create(&th_adder, NULL, thread_adder, &maxn);
    if (s != 0) {
        pexit("pthread_create(ADDER)", s);
    }

    s = pthread_create(&th_deler, NULL, thread_deler, &maxn);
    if (s != 0) {
        pexit("pthread_create(DELER)", s);
    }

    s = pthread_create(&th_looker, NULL, thread_looker, &maxn);
    if (s != 0) {
        pexit("pthread_create(LOOKER)", s);
    }

    s = pthread_join(th_adder, NULL);
    if (s != 0) {
        pexit("pthread_join(ADDER)", s);
    }

    s = pthread_join(th_deler, NULL);
    if (s != 0) {
        pexit("pthread_join(DELER)", s);
    }

    s = pthread_join(th_looker, NULL);
    if (s != 0) {
        pexit("pthread_join(LOOKER)", s);
    }

    exit(EXIT_SUCCESS);
}
