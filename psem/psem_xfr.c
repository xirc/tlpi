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
#include <semaphore.h>
#include <pthread.h>


#define BUF_SIZE 1024
static sem_t g_sem_reader;
static sem_t g_sem_writer;
char g_buffer[BUF_SIZE];
long g_buflen;


static void *
thread_reader(void *arg __attribute__((unused)))
{
    unsigned long xfrs;
    unsigned long bytes;

    /* Transfer blocks of data from global buffer to stdout */
    for (xfrs = 0, bytes = 0; /* do nothing */; xfrs++) {
        /* Wait for our turn */
        if (sem_wait(&g_sem_reader) == -1) {
            perror("R: sem_wait");
            exit(EXIT_FAILURE);
        }

        /* Writer encounterd EOF */
        if (g_buflen == 0) {
            break;
        }
        bytes += g_buflen;

        if (write(STDOUT_FILENO, g_buffer, g_buflen) != g_buflen) {
            perror("partial/failed write");
            exit(EXIT_FAILURE);
        }

        /* Give writer a turn */
        if (sem_post(&g_sem_writer) == -1) {
            perror("R: sem_post");
            exit(EXIT_FAILURE);
        }
    }

    fprintf(stderr, "Received %ld bytes (%ld xfrs)\n", bytes, xfrs);
    return NULL;
}


static void *
thread_writer(void *arg __attribute__((unused)))
{
    unsigned long xfrs;
    unsigned long bytes;

    /* Transfer blocks of data from stdin to global buffer */
    for (xfrs = 0, bytes = 0;
            /* do nothing */;
            ++xfrs, bytes += g_buflen)
    {
        /* Wait for our turn */
        if (sem_wait(&g_sem_writer) == -1) {
            perror("W: sem_wait");
            exit(EXIT_FAILURE);
        }

        g_buflen = read(STDIN_FILENO, g_buffer, BUF_SIZE);
        if (g_buflen == -1) {
            perror("W: read");
            exit(EXIT_FAILURE);
        }

        /* Give reader a turn */
        if (sem_post(&g_sem_reader) == -1) {
            perror("W: sem_post");
            exit(EXIT_FAILURE);
        }

        /* Have we reached EOF? We test this after giving the reader
         * a turn so that it can see the 0 value in g_buflen. */
        if (g_buflen == 0) {
            break;
        }
    }

    fprintf(stderr, "Sent %ld bytes (%ld xfrs)\n", bytes, xfrs);
    return NULL;
}


int
main(int argc __attribute__((unused)),
     char *argv[] __attribute__((unused)))
{
    pthread_t t_reader, t_writer;
    int s;

    if (sem_init(&g_sem_reader, 0, 0) == -1) {
        perror("sem_init - reader");
        exit(EXIT_FAILURE);
    }
    if (sem_init(&g_sem_writer, 0, 1) == -1) {
        perror("sem_init - writer");
        exit(EXIT_FAILURE);
    }


    /* Make reader/writer threads */
    s = pthread_create(&t_reader, NULL, thread_reader, NULL);
    if (s != 0) {
        errno = s;
        perror("pthread_create - reader");
        exit(EXIT_FAILURE);
    }
    s = pthread_create(&t_writer, NULL, thread_writer, NULL);
    if (s != 0) {
        errno = s;
        perror("pthread_create - writer");
        exit(EXIT_FAILURE);
    }

    /* Wait for reader/writer threads */
    s = pthread_join(t_reader, NULL);
    if (s != 0) {
        errno = s;
        perror("pthread_join - reader");
        exit(EXIT_FAILURE);
    }
    s = pthread_join(t_writer, NULL);
    if (s != 0) {
        errno = s;
        perror("pthread_join - writer");
        exit(EXIT_FAILURE);
    }

    if (sem_destroy(&g_sem_reader) == -1) {
        perror("sem_destroy");
        exit(EXIT_FAILURE);
    }
    if (sem_destroy(&g_sem_writer) == -1) {
        perror("sem_destroy");
        exit(EXIT_FAILURE);
    }
    exit(EXIT_SUCCESS);
}
