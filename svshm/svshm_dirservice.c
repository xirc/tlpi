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
#include <sys/shm.h>
#include <sys/sem.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <string.h>

#include "semun.h"
#include "svshm_dirservice.h"


#define SEM_KEY 14102221
#define OBJ_PERMS (S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP)
#define SHM_KEY 14102221
#define NMAXENT 8096


static int shmid;                   /* Shared memory ID */
static int semid;                   /* Semaphore ID */
static void *shmp;                  /* Pointer to allocated shared memory */
static size_t *num_entries;         /* Number of entries */
static struct shment *entries;      /* Entries */
static size_t const sizeof_shmtab = /* Size of allocated memory */
    sizeof(num_entries) + sizeof(struct shment) * NMAXENT;
static int cursor = -1;             /* Cursor to point shment */


/*
 * Reserve semaphore
 * Return 0 on success, or -1 on error.
 */
static int
reserve()
{
    struct sembuf sops;

    sops.sem_num = 0;
    sops.sem_op = -1;
    sops.sem_flg = SEM_UNDO;

    while (semop(semid, &sops, 1) == -1) {
        if (errno != EINTR) {
            return -1;
        }
    }
    return 0;
}


/*
 * Release semaphore
 * Return 0 on success, or -1 on error.
 */
static int
release()
{
    struct sembuf sops;

    sops.sem_num = 0;
    sops.sem_op = 1;
    sops.sem_flg = SEM_UNDO;

    while (semop(semid, &sops, 1) == -1) {
        if (errno != EINTR) {
            return -1;
        }
    }
    return 0;
}


/*
 * Setup shment service.
 * Return 0 on success, or -1 on error.
 * If 'is_create' is 1,
 * create new semaphore and new shared memory.
 */
int
shment_setup(int is_create)
{
    union semun arg;
    int flag;

    /* Create or not */
    flag = is_create ? IPC_CREAT | IPC_EXCL : 0;

    /* Create or get a semahpre */
    semid = semget(SEM_KEY, 1, flag | OBJ_PERMS);
    if (shmid == -1) {
        return -1;
    }
    arg.val = 1;
    if (semctl(semid, 0, SETVAL, arg) == -1) {
        return -1;
    }

    /* Create or get a shared memory */
    shmid = shmget(SHM_KEY, sizeof_shmtab, flag | OBJ_PERMS);
    if (shmid == -1) {
        return -1;
    }
    /* Attach shared memory */
    shmp = shmat(shmid, NULL, 0);
    if (shmp == (void*) -1) {
        return -1;
    }
    num_entries = shmp;
    entries = shmp + sizeof(num_entries);

    errno = 0;
    return 0;
}


/*
 * Cleanup shment service.
 * Return 0 on success, or -1 on error.
 * If 'is_delete' is 1,
 * delete the semaphore and the shared memory in system'.
 */
int
shment_cleanup(int is_delete)
{
    union semun dummy;

    /* Delete semaphore */
    if (is_delete) {
        if (semctl(semid, 0, IPC_RMID, dummy) == -1) {
            return -1;
        }
    }
    /* Detach shared memory */
    if (shmdt(shmp) == -1) {
        return -1;
    }
    /* Delete shared memory */
    if (is_delete) {
        if (shmctl(shmid, IPC_RMID, 0) == -1) {
            return -1;
        }
    }

    errno = 0;
    return 0;
}


/*
 * Create shment entry.
 * Return 0 on success, return -1 on error.
 * If 'path' is NULL, return -1 and errno = EINVAL.
 * If the entry exists already or the table is full,
 * return -1 and errno = 0.
 */
int
shment_create(char const *path, void const *value)
{
    size_t i;
    int saved_errno = 0;

    if (path == NULL) {
        errno = EINVAL;
        return -1;
    }

    if (reserve() == -1) {
        return -1;
    }

    /* Full */
    if (*num_entries >= NMAXENT) {
        saved_errno = 0;
        goto FAIL;
    }

    for (i = 0; i < *num_entries; ++i) {
        if (strcmp(entries[i].path, path) == 0) {
            /* The entry with 'path' exists already */
            saved_errno = 0;
            goto FAIL;
        }
    }
    /* Make new entry with 'path' and 'value'. */
    (void) strncpy(entries[i].path, path, PATH_MAX);
    entries[i].path[PATH_MAX - 1] = '\0';
    entries[i].value = value;
    *num_entries += 1;

    if (release() == -1) {
        return -1;
    }

    errno = 0;
    return 0;

FAIL:
    if (release() == -1) {
        return -1;
    }
    errno = saved_errno;
    return -1;
}


/*
 * Move shment entry from 'old-path' to 'new-path'.
 * Return 0 on success, or -1 on error.
 * If either 'old-path' or 'new-path' is NULL,
 * return -1, and errno = EINVAL.
 * If the entry with 'old-path' does not exists,
 * return -1 and errno = 0.
 * If the entry with 'new-path' exists already,
 * return -1 and errno = 0.
 * */
int
shment_move(char const *old_path, char const *new_path)
{
    size_t i, old_idx, new_idx;
    int saved_errno = 0;

    if (old_path == NULL || new_path == NULL) {
        errno = EINVAL;
        return -1;
    }

    if (reserve() == -1) {
        return -1;
    }

    old_idx = NMAXENT;
    new_idx = NMAXENT;
    for (i = 0; i < *num_entries; ++i) {
        if (strcmp(entries[i].path, old_path) == 0) {
            old_idx = i;
        }
        if (strcmp(entries[i].path, new_path) == 0) {
            new_idx = i;
        }
    }
    if (old_idx == NMAXENT) {
        /* Old entry does not exists */
        saved_errno = 0;
        goto FAIL;
    }
    if (new_idx != NMAXENT) {
        /* New entry exists already */
        saved_errno = 0;
        goto FAIL;
    }

    /* Move the entry */
    i = old_idx;
    (void) strncpy(entries[i].path, new_path, PATH_MAX);
    entries[i].path[PATH_MAX - 1] = '\0';

    if (release() == -1) {
        return -1;
    }

    errno = 0;
    return 0;

FAIL:
    if (release() == -1) {
        return -1;
    }
    errno = saved_errno;
    return -1;
}


/*
 * Remove shment entry with 'path'.
 * Return 0 on success, or -1 on error.
 * If the entry does not exits, return -1 errno = 0.
 * If the entry is removed, the value of the entry is stored in 'value'.
 */
int
shment_remove(char const *path, void const **value)
{
    size_t i;
    int saved_errno = 0;

    if (path == NULL || value == NULL) {
        errno = EINVAL;
        return -1;
    }

    if (reserve() == -1) {
        return -1;
    }

    for (i = 0; i < *num_entries; ++i) {
        if (strcmp(entries[i].path, path) == 0) {
            break;
        }
    }
    if (i == *num_entries) {
        saved_errno = 0;
        goto FAIL;
    }
    if (value != NULL) {
        *value = entries[i].value;
    }
    (void) strncpy(entries[i].path, entries[*num_entries-1].path, PATH_MAX);
    entries[i].path[PATH_MAX - 1] = '\0';
    *num_entries -= 1;

    if (release() == -1) {
        return -1;
    }

    errno = 0;
    return 0;

FAIL:
    if (release() == -1) {
        return -1;
    }
    errno = saved_errno;
    return -1;
}


/*
 * Set or get the value to entry with 'path'.
 * Return 0 on success, or -1 on error.
 * If 'path' is NULL, return -1 and errno = EINVAL.
 * If the entry with 'path' does not exists, return -1 and errno = 0.
 * If both 'new-value' and 'old-value' is NULL, return -1 and errno = EINVAL.
 * This API behaves like sigaction(SIG, NEW, OLD).
 */
int
shment_ctl_value(char const *path,
        void const **new_value, void const **old_value)
{
    size_t i;
    int saved_errno = 0;

    if (path == NULL) {
        errno = EINVAL;
        return -1;
    }
    if (new_value == NULL && old_value == NULL) {
        errno = EINVAL;
        return -1;
    }

    if (reserve() == -1) {
        return -1;
    }

    for (i = 0; i < *num_entries; ++i) {
        if (strcmp(entries[i].path, path) == 0) {
            break;
        }
    }
    if (i == *num_entries) {
        saved_errno = 0;
        goto FAIL;
    }
    if (old_value != NULL) {
        *old_value = entries[i].value;
    }
    if (new_value != NULL) {
        entries[i].value = *new_value;
    }

    if (release() == -1) {
        return -1;
    }
    errno = 0;
    return 0;

FAIL:
    if (release() == -1) {
        return -1;
    }
    errno = saved_errno;
    return -1;
}


/*
 * Get shmentry pointed by cursor.
 * If it reach end of entries, return NULL.
 */
struct shment * shment_get(void)
{
    static struct shment entry;

    /* If cursor is not initialized, then initialize it */
    if (cursor == -1) {
        cursor = 0;
    }

    /* Reach END of table */
    if (cursor >= (int)*num_entries) {
        return NULL;
    }

    (void) memcpy(&entry, entries+cursor, sizeof(struct shment));
    cursor++;
    return &entry;
}


/*
 * Close cursor.
 */
void shment_end(void)
{
    cursor = -1;
}
