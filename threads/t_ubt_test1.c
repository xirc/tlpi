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
#include <stdlib.h>
#include <assert.h>
#include "ubt.h"


int
main(int argc __attribute__((unused)),
     char *argv[] __attribute__((unused)))
{
    struct Node tree;
    int s;
    void *value;

    /* INITIALIZE */
    s = initialize(&tree);
    assert(s == 0);

    /* INITIALIZE FAILURE */
    s = initialize(NULL);
    assert(s != 0);

    /* INSERT */
    s = add(&tree, "f", (void*)600);
    assert(s == 0);
    s = add(&tree, "b", (void*)200);
    assert(s == 0);
    s = add(&tree, "e", (void*)500);
    assert(s == 0);
    s = add(&tree, "g", (void*)700);
    assert(s == 0);
    s = add(&tree, "d", (void*)400);
    assert(s == 0);
    s = add(&tree, "a", (void*)100);
    assert(s == 0);
    s = add(&tree, "h", (void*)800);
    assert(s == 0);

    /* INSERT FAILURE */
    s = add(&tree, "a", (void*)100);
    assert(s == -1);
    s = add(NULL, "a", (void*)0);
    assert(s != -1 && s != 0);
    s = add(&tree, NULL, (void*)0);
    assert(s != -1 && s != 0);

    /* LOOKUP */
    s = lookup(&tree, "e", &value);
    assert(s == 0);
    assert((int)value == 500);
    s = lookup(&tree, "g", &value);
    assert(s == 0);
    assert((int)value == 700);

    /* LOOKUP FAILURE */
    s = lookup(&tree, "z", &value);
    assert(s == -1);
    s = lookup(NULL, "z", &value);
    assert(s != -1 && s != 0);
    s = lookup(&tree, NULL, &value);
    assert(s != -1 && s != 0);
    s = lookup(&tree, "z", NULL);
    assert(s != -1 && s != 0);

    /* DELETE TREE */
    s = delete(&tree, "e");
    assert(s == 0);
    s = delete(&tree, "g");
    assert(s == 0);
    s = delete(&tree, "d");
    assert(s == 0);
    s = delete(&tree, "f");
    assert(s == 0);

    /* DELETE FAILURE */
    s = delete(&tree, "z");
    assert(s == -1);
    s = delete(&tree, NULL);
    assert(s != -1 && s != 0);
    s = delete(NULL, "z");
    assert(s != -1 && s != 0);

    /* PRINT_TREE */
    s = print_tree(&tree);
    assert(s == 0);

    /* PRINT_TREE FAILURE*/
    s = print_tree(NULL);
    assert(s != 0);

    exit(EXIT_SUCCESS);
}
