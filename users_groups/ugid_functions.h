/*************************************************************************\
*                  Copyright (C) Michael Kerrisk, 2014.                   *
*                                                                         *
* This program is free software. You may use, modify, and redistribute it *
* under the terms of the GNU Lesser General Public License as published   *
* by the Free Software Foundation, either version 3 or (at your option)   *
* any later version. This program is distributed without any warranty.    *
* See the files COPYING.lgpl-v3 and COPYING.gpl-v3 for details.           *
\*************************************************************************/
/*************************************************************************\
*                  Copyright (C) xirc, 2014.                              *
* See above.                                                              *
\*************************************************************************/


/* ugid_functions.h

   Header file for ugid_functions.c.
*/


#ifndef UGID_FUNCTIONS_H
#define UGID_FUNCTIONS_H


char *user_name_from_id(uid_t uid);

uid_t user_id_from_name(const char *name);

char *group_name_from_id(gid_t gid);

gid_t group_id_from_name(const char *name);


#endif
