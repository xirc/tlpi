/*************************************************************************\
*                  Copyright (C) xirc, 2014.                              *
*                                                                         *
* This program is free software. You may use, modify, and redistribute it *
* under the terms of the GNU Affero General Public License as published   *
* by the Free Software Foundation, either version 3 or (at your option)   *
* any later version. This program is distributed without any warranty.    *
* See the file COPYING.agpl-v3 for details.                               *
\*************************************************************************/


#ifndef SVSHM_DIRSERVICE_H
#define SVSHM_DIRSERVICE_H


#include <limits.h>


struct shment {                     /* Entry */
    char path[PATH_MAX];            /* Path (identifier) */
    void const *value;              /* Value             */
};


int shment_setup(int is_create);
int shment_cleanup(int is_delete);
int shment_create(char const *path, void const *value);
int shment_move(char const *old_path, char const *new_path);
int shment_remove(char const *path, void const **value);
int shment_ctl_value(char const *path,
        void const **new_value, void const **old_value);
struct shment * shment_get(void);
void shment_end(void);


#endif
