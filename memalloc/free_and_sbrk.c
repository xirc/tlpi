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


/* free_and_sbrk.c

   Test if free(3) actually lowers the program break.

   Usage: free_and_sbrk num-allocs block-size [step [min [max]]]

   Try: free_and_sbrk 1000 10240 2 1 1000
        free_and_sbrk 1000 10240 1 1 999
        free_and_sbrk 1000 10240 1 500 1000

        (Only the last of these should see the program break lowered.)
*/


#define _BSD_SOURCE
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>


#define MAX_ALLOCS 1000000


static void
usage(FILE *fp, const char *progname)
{
    fprintf(fp,
        "Usage: %s num-allocs block-size [step [min [max]]]\n", progname);
}


int
main(int argc, char *argv[])
{
    char *ptr[MAX_ALLOCS];
    unsigned int free_step, free_min, free_max, block_size, num_allocs, j;


    if (argc == 2 && strcmp(argv[1], "--help") == 0) {
        usage(stdout, argv[0]);
        exit(EXIT_SUCCESS);
    }
    if (argc < 3) {
        usage(stderr, argv[0]);
        exit(EXIT_FAILURE);
    }

    num_allocs = strtoul(argv[1], NULL, 0);
    if (num_allocs > MAX_ALLOCS) {
        fprintf(stderr, "num-allocs > %d\n", MAX_ALLOCS);
        usage(stderr, argv[0]);
        exit(EXIT_FAILURE);
    }

    block_size = strtoul(argv[2], NULL, 0);

    free_step = (argc > 3) ? strtoul(argv[3], NULL, 0) : 1;
    free_min =  (argc > 4) ? strtoul(argv[4], NULL, 0) : 1;
    free_max =  (argc > 5) ? strtoul(argv[5], NULL, 0) : num_allocs;

    if (free_max > num_allocs) {
        fprintf(stderr, "free-max > num-allocs\n");
        usage(stderr, argv[0]);
        exit(EXIT_FAILURE);
    }

    printf("Initial program break:          %10p\n", sbrk(0));

    printf("Allocating %d*%d bytes\n", num_allocs, block_size);
    for (j = 0; j < num_allocs; j++) {
        ptr[j] = malloc(block_size);
        if (ptr[j] == NULL) {
            perror("malloc");
            exit(EXIT_FAILURE);
        }
    }

    printf("Program break is now:           %10p\n", sbrk(0));

    printf("Freeing blocks from %d to %d in steps of %d\n",
                free_min, free_max, free_step);
    for (j = free_min - 1; j < free_max; j += free_step) {
        free(ptr[j]);
    }

    printf("After free(), program break is: %10p\n", sbrk(0));

    exit(EXIT_SUCCESS);
}
