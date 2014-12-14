/*************************************************************************\
*                  Copyright (C) xirc, 2014.                              *
*                                                                         *
* This program is free software. You may use, modify, and redistribute it *
* under the terms of the GNU Affero General Public License as published   *
* by the Free Software Foundation, either version 3 or (at your option)   *
* any later version. This program is distributed without any warranty.    *
* See the file COPYING.agpl-v3 for details.                               *
\*************************************************************************/


#ifndef CAPLIB_H
#define CAPLIB_H


/* Change setting of capability
 * in caller's effective capabilities */
int
modify_cap(int capability, int setting);


/* Raise capability in caller's effective set */
int
raise_cap(int capability);


/* Drop capability in caller's effective set */
int
drop_cap(int capability);


/* Drop all capabilities from all sets */
int
drop_all_caps(void);


#endif
