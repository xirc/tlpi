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


/* rdwrn.h

   Header file for rdwrn.c.
*/


#ifndef RDWRN_H
#define RDWRN_H

#include <sys/types.h>


ssize_t readn(int fd, void *buffer, size_t count);

ssize_t writen(int fd, const void *buffer, size_t count);


#endif
