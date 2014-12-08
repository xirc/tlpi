/*************************************************************************\
*                  Copyright (C) xirc, 2014.                              *
*                                                                         *
* This program is free software. You may use, modify, and redistribute it *
* under the terms of the GNU Affero General Public License as published   *
* by the Free Software Foundation, either version 3 or (at your option)   *
* any later version. This program is distributed without any warranty.    *
* See the file COPYING.agpl-v3 for details.                               *
\*************************************************************************/


#define _BSD_SOURCE
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>


static void*
m_malloc(size_t size);


static void
m_free(void *ptr);


static void
print_free_list();


int
main(int argc __attribute__((unused)),
     char *argv[] __attribute__((unused)))
{
    printf("initial break:      %p\n", sbrk(0));
    void *ptr[4];
    ptr[0] = m_malloc(0x10);
    ptr[1] = m_malloc(0x20);
    ptr[2] = m_malloc(0x30);
    ptr[3] = m_malloc(0x40);

    print_free_list();
    m_free(ptr[2]);
    print_free_list();
    m_free(ptr[0]);
    print_free_list();
    m_free(ptr[3]);
    print_free_list();

    ptr[0] = m_malloc(0x20);
    print_free_list();

    m_free(ptr[1]);
    print_free_list();

    m_free(ptr[0]);
    print_free_list();

    exit(EXIT_SUCCESS);
}



/*
 * malloc and free
 */
struct free_list_t {
    size_t size;
    struct free_list_t *prev;
    struct free_list_t *next;
};
static struct free_list_t *free_list;
static void* last_address;


static void*
m_malloc(size_t size)
{
    /* verbose */
    printf("my_malloc\n");

    /* search fit element of free list */
    if (free_list != NULL) {
        struct free_list_t *elem;

        for (elem = free_list; elem != last_address; elem = elem->next) {
            if (elem->size == size) {
                /* elem is just fit */
                struct free_list_t *prev, *next;
                prev = elem->prev;
                next = elem->next;
                if (next == last_address &&
                    prev == last_address)
                {
                    /* this element is the first and the last */
                    last_address -= elem->size + sizeof(int);
                    free_list = NULL;
                } else if (prev == last_address) {
                    /* this element is the first */
                    free_list = next;
                } else if (next == last_address) {
                    /* this element is the last */
                    last_address -= elem->size + sizeof(int);
                    free_list->prev = last_address;
                }
                return (void*)((size_t)elem + sizeof(int));
            } else if (elem->size > size) {
                /* elem is fit, but not just size */
                struct free_list_t *rem_elem, *next_elem, *prev_elem;
                rem_elem = (void*)((size_t)elem + sizeof(int) + size);
                prev_elem = elem->prev;
                next_elem = elem->next;

                rem_elem->size = elem->size - size - sizeof(int);
                rem_elem->prev = prev_elem;
                rem_elem->next = next_elem;

                if (rem_elem->next == last_address &&
                    rem_elem->prev == last_address)
                {
                    /* this element is the first and last */
                    last_address -= size + sizeof(int);
                    free_list = rem_elem;
                    rem_elem->prev = last_address;
                    rem_elem->next = last_address;
                } else if (prev_elem == last_address) {
                    /* this element is the first */
                    free_list = rem_elem;
                    next_elem->prev = rem_elem;
                } else if (next_elem == last_address) {
                    /* this element is the last */
                    prev_elem->next = rem_elem;
                } else {
                    /* this element is not the first and the last */
                    prev_elem->next = rem_elem;
                    next_elem->prev = rem_elem;
                }
                elem->size = size;
                return (void*)((size_t)elem + sizeof(int));
            }
        }
    }

    /* allocate memory from new heap */
    struct free_list_t *elem;
    size_t base_addr;
    elem = sbrk(size + sizeof(int));
    if (elem == (void*)-1) {
        return NULL;
    }
    elem->size = size;
    base_addr = (size_t)elem + sizeof(int);
    /* verbose */
    printf("sbrk: %p (%p)\n\n", (void*)base_addr, elem);

    return (void*)base_addr;
}


static void
m_free(void *ptr)
{
    struct free_list_t *free_elem, *elem;
    free_elem = (struct free_list_t*) ((size_t)ptr - sizeof(int));

    /* verbose */
    printf("my_free\n");
    printf("free: %p, size %d\n\n", free_elem, free_elem->size);

    /* free_list is empty */
    if (free_list == NULL) {
        free_list = free_elem;
        /* free_elem->size: already initialized */
        last_address = (void*) ((size_t)free_elem + sizeof(int) + free_elem->size);
        free_elem->prev = last_address;
        free_elem->next = last_address;
        return;
    }

    /*
     * free_elem is the first
     * [HERE free_list...]
     */
    if (free_elem < free_list) {
        if ((size_t)free_elem + sizeof(int) + free_elem->size == (size_t)free_list)
        {
            /* merge */
            free_elem->size = free_elem->size + free_list->size + sizeof(int);
            free_elem->prev = last_address;
            free_elem->next = free_list->next;
            free_list = free_elem;
        }
        else
        {
            /* insert */
            /* free_elem->size: already initialized */
            free_elem->prev = last_address;
            free_elem->next = free_list;
            free_list->prev = free_elem;
            free_list = free_elem;
        }
        return;
    }

    /*
     * search insert point
     * free_elem elem  [... elem->prev HERE1 elem HERE2 ...]
     */
    for (elem = free_list; elem->next != last_address; elem = elem->next) {
        if (elem > free_elem) break;
    }

    /*
     * freed element is the last
     * [... elem HERE]
     */
    if (elem->next == last_address && free_elem > elem) {
        last_address = (void*) ((size_t)free_elem + sizeof(int) + free_elem->size);
        if ((size_t)elem + sizeof(int) + elem->size == (size_t) free_elem)
        {
            /* merge */
            elem->size = elem->size + free_elem->size + sizeof(int);
            /* elem->prev: no change */
            elem->next = last_address;
            free_list->prev = last_address;
        }
        else
        {
            /* insert */
            elem->next = free_elem;
            /* free_elem->size: already initialized */
            free_elem->prev = elem;
            free_elem->next = last_address;
            free_list->prev = last_address;
        }
        return;
    }

    /*
     * freed element is not the first and last
     * [... elem->prev HERE elem ...]
     */
    {
        int is_merge_prev =
            (size_t)elem->prev + sizeof(int) + elem->prev->size == (size_t) free_elem;
        int is_merge_next =
            (size_t)free_elem + sizeof(int) + free_elem->size == (size_t)elem;
        if (is_merge_prev && is_merge_next) {
            elem->prev->size += free_elem->size + elem->size + sizeof(int) * 2;
            elem->prev->next = last_address;
            return;
        }
        if (is_merge_prev) {
            elem->prev->size += free_elem->size + sizeof(int);
            return;
        }
        if (is_merge_next) {
            elem->size += free_elem->size + sizeof(int);
            return;
        }
        /* free_elem->size: already initialized */
        free_elem->prev = elem->prev;
        free_elem->next = elem;
        free_elem->prev->next = free_elem;
        free_elem->next->prev = free_elem;
        return;
    }
}


static void
print_free_list()
{
    int i;
    struct free_list_t *e;

    printf("last address: %p\n", last_address);
    printf("free list: \n");

    /* free_list is empty */
    if (free_list == NULL) {
        printf("\n");
        return;
    }

    /* free_list is not empty */
    for (e = free_list, i = 0; e != last_address; e = e->next, ++i) {
        printf("  %d: item(%p), size(%d), prev(%p), next(%p)\n",
                i, e, e->size, e->prev, e->next);
    }
    printf("\n");
}
