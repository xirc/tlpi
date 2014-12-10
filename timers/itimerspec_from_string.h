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


/* itimerspec_from_string.h

   Header file for itimerspec_from_string.c.
*/


#ifndef ITIMERSPEC_FROM_STRING_H
#define ITIMERSPEC_FROM_STRING_H


#include <time.h>


void itimerspec_from_string(const char *str, struct itimerspec *tsp);


#endif
